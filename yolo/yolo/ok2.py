#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from custom_msgs.msg import MarkerTag
import numpy as np
import cv2
import tensorrt as trt
import pycuda.driver as cuda
import pycuda.autoinit
from flask import Flask, Response
import threading
import time

# ---------------- Flask ----------------
app = Flask(__name__)
latest_frame = None

@app.route("/video")
def video():
    def gen():
        global latest_frame
        while True:
            if latest_frame is None:
                time.sleep(0.01)
                continue

            ok, jpg = cv2.imencode(
                ".jpg",
                latest_frame,
                [cv2.IMWRITE_JPEG_QUALITY, 70]
            )
            if not ok:
                continue

            yield (b"--frame\r\n"
                   b"Content-Type: image/jpeg\r\n\r\n" +
                   jpg.tobytes() + b"\r\n")

            time.sleep(0.05)  # ~20 FPS stream
    return Response(gen(), mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0", port=5001, debug=False, threaded=True)

# ---------------- TensorRT ----------------
class TRTInfer:
    def __init__(self, engine_path):
        logger = trt.Logger(trt.Logger.WARNING)
        with open(engine_path, "rb") as f, trt.Runtime(logger) as runtime:
            self.engine = runtime.deserialize_cuda_engine(f.read())

        self.context = self.engine.create_execution_context()
        self.stream = cuda.Stream()

        self.input_name = self.engine.get_tensor_name(0)
        self.output_name = self.engine.get_tensor_name(1)

        self.input_shape = self.engine.get_tensor_shape(self.input_name)
        self.output_shape = self.engine.get_tensor_shape(self.output_name)

        self.d_input = cuda.mem_alloc(
            trt.volume(self.input_shape) * np.float32().nbytes
        )
        self.d_output = cuda.mem_alloc(
            trt.volume(self.output_shape) * np.float32().nbytes
        )

        self.context.set_tensor_address(self.input_name, int(self.d_input))
        self.context.set_tensor_address(self.output_name, int(self.d_output))

        self.h_output = np.empty(self.output_shape, dtype=np.float32)

    def infer(self, img):
        img = np.ascontiguousarray(img, dtype=np.float32)
        cuda.memcpy_htod_async(self.d_input, img, self.stream)
        self.context.execute_async_v3(self.stream.handle)
        cuda.memcpy_dtoh_async(self.h_output, self.d_output, self.stream)
        self.stream.synchronize()
        return self.h_output

# ---------------- ROS Node ----------------
class InferenceEngine(Node):
    def __init__(self):
        super().__init__("inference_engine")

        self.bridge = CvBridge()
        self.trt = TRTInfer(
            "/home/mrmnavjet/IRC2026/ircWS/src/yolo/yolo/cone_v1.engine"
        )

        self.latest_rgb = None
        self.latest_depth = None

        self.COLOR_ID = {
            "orange": 1,
            "red": 2,
            "blue": 3,
            "green": 4,
            "yellow": 5
        }

        self.sub_rgb = self.create_subscription(
            Image,
            "/zed/zed_node/rgb/image_rect_color",
            self.rgb_cb,
            10
        )
        self.sub_depth = self.create_subscription(
            Image,
            "/zed/zed_node/depth/depth_registered",
            self.depth_cb,
            10
        )

        self.pub = self.create_publisher(MarkerTag, "/marker_detect", 10)

        self.timer = self.create_timer(0.033, self.process)

        self.fps_start_time = time.time()
        self.fps_frame_count = 0
        self.current_fps = 0.0
        self.last_log_time = time.time()

        threading.Thread(target=start_flask, daemon=True).start()
        self.get_logger().info(
            "Inference engine started | Flask: http://0.0.0.0:5001/video"
        )

    # ---------------- Callbacks ----------------
    def rgb_cb(self, msg):
        self.latest_rgb = msg

    def depth_cb(self, msg):
        self.latest_depth = msg

    # ---------------- Depth ----------------
    def calculate_distance(self, depth, cx, cy):
        h, w = depth.shape
        s = 10
        x1 = max(0, cx - s)
        x2 = min(w, cx + s)
        y1 = max(0, cy - s)
        y2 = min(h, cy + s)

        patch = depth[y1:y2, x1:x2]
        if patch.size == 0:
            return None

        patch = patch[np.isfinite(patch)]
        patch = patch[(patch > 0.1) & (patch < 20.0)]
        if patch.size == 0:
            return None

        return float(np.median(patch))

    # ---------------- FAST COLOR ----------------
    def detect_color_fast(self, frame, cx, cy):
        h, w, _ = frame.shape
        s = 6

        x1 = max(0, cx - s)
        x2 = min(w, cx + s)
        y1 = max(0, cy - s)
        y2 = min(h, cy + s)

        patch = frame[y1:y2, x1:x2]
        if patch.size == 0:
            return None

        hsv = cv2.cvtColor(patch, cv2.COLOR_BGR2HSV)
        h_mean, s_mean, v_mean = np.mean(
            hsv.reshape(-1, 3), axis=0
        )

        if s_mean < 60 or v_mean < 60:
            return None

        if 5 <= h_mean <= 20:
            return "orange"
        if h_mean <= 10 or h_mean >= 170:
            return "red"
        if 90 <= h_mean <= 130:
            return "blue"
        if 40 <= h_mean <= 80:
            return "green"
        if 22 <= h_mean <= 34:
            return "yellow"

        return None

    # ---------------- IoU + NMS ----------------
    def calculate_iou(self, b1, b2):
        x1 = max(b1[0], b2[0])
        y1 = max(b1[1], b2[1])
        x2 = min(b1[2], b2[2])
        y2 = min(b1[3], b2[3])

        inter = max(0, x2 - x1) * max(0, y2 - y1)
        a1 = (b1[2] - b1[0]) * (b1[3] - b1[1])
        a2 = (b2[2] - b2[0]) * (b2[3] - b2[1])
        u = a1 + a2 - inter

        return 0 if u == 0 else inter / u

    def nms(self, dets, thr=0.5):
        dets = sorted(dets, key=lambda x: x[4], reverse=True)
        keep = []
        while dets:
            best = dets.pop(0)
            keep.append(best)
            dets = [
                d for d in dets
                if self.calculate_iou(best[:4], d[:4]) < thr
            ]
        return keep

    # ---------------- Main Loop ----------------
    def process(self):
        global latest_frame

        if self.latest_rgb is None or self.latest_depth is None:
            return

        self.fps_frame_count += 1
        now = time.time()
        if now - self.fps_start_time > 1.0:
            self.current_fps = self.fps_frame_count / (now - self.fps_start_time)
            self.fps_frame_count = 0
            self.fps_start_time = now

        frame = self.bridge.imgmsg_to_cv2(self.latest_rgb, "bgr8")
        depth = self.bridge.imgmsg_to_cv2(self.latest_depth)

        if depth.dtype == np.uint16:
            depth = depth.astype(np.float32) * 0.001

        img = cv2.resize(frame, (640, 640))
        img = img.transpose(2, 0, 1)[None] / 255.0

        output = self.trt.infer(img)[0]

        h, w, _ = frame.shape
        sx, sy = w / 640.0, h / 640.0
        img_center = w / 2.0

        # ---- Confidence filter + top-K ----
        candidates = []
        for det in output.T:
            if det[4] < 0.50:
                continue

            cx = int(det[0] * sx)
            cy = int(det[1] * sy)
            bw = int(det[2] * sx)
            bh = int(det[3] * sy)

            x1 = max(0, cx - bw // 2)
            y1 = max(0, cy - bh // 2)
            x2 = min(w, cx + bw // 2)
            y2 = min(h, cy + bh // 2)

            candidates.append((x1, y1, x2, y2, det[4]))

        candidates.sort(key=lambda x: x[4], reverse=True)
        candidates = candidates[:2]

        detections = self.nms(candidates)

        published = 0
        for x1, y1, x2, y2, conf in detections:
            cx = (x1 + x2) // 2
            cy = (y1 + y2) // 2

            color = self.detect_color_fast(frame, cx, cy)
            if color is None:
                continue

            dist = self.calculate_distance(depth, cx, cy)
            if dist is None:
                continue

            y_offset = (cx - img_center) / 558.0

            msg = MarkerTag()
            msg.is_found = True
            msg.id = self.COLOR_ID[color]
            msg.x = dist
            msg.y = -y_offset
            self.pub.publish(msg)
            published += 1

            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 255), 2)
            cv2.putText(
                frame,
                f"{color} {dist:.2f}m",
                (x1, y1 - 8),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 255, 255),
                2
            )

        if now - self.last_log_time > 2.0:
            self.get_logger().info(
                f"[PERF] FPS: {self.current_fps:.1f} | "
                f"Detections: {published} | ConfThr: 0.50"
            )
            self.last_log_time = now

        cv2.putText(
            frame,
            f"FPS: {self.current_fps:.1f}",
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            1.0,
            (0, 255, 0),
            2
        )

        latest_frame = frame

# ---------------- main ----------------
def main():
    rclpy.init()
    node = InferenceEngine()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()
