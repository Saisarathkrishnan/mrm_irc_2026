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
            ok, jpg = cv2.imencode(".jpg", latest_frame)
            if not ok:
                continue
            yield (b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + jpg.tobytes() + b"\r\n")
    return Response(gen(), mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0", port=5001, debug=False, threaded=True)

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
        self.d_input = cuda.mem_alloc(trt.volume(self.input_shape) * np.float32().nbytes)
        self.d_output = cuda.mem_alloc(trt.volume(self.output_shape) * np.float32().nbytes)
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

class InferenceEngine(Node):
    def __init__(self):
        super().__init__("inference_engine")
        self.bridge = CvBridge()
        self.trt = TRTInfer("/home/mrmnavjet/IRC2026/ircWS/src/yolo/yolo/cone_v1.engine")
        self.latest_rgb = None
        self.latest_depth = None
        self.COLOR_ID = {"orange": 1, "red": 2, "blue": 3, "green": 4, "yellow": 5}
        self.sub_rgb = self.create_subscription(Image, "/zed/zed_node/rgb/image_rect_color", self.rgb_cb, 10)
        self.sub_depth = self.create_subscription(Image, "/zed/zed_node/depth/depth_registered", self.depth_cb, 10)
        self.pub = self.create_publisher(MarkerTag, "/marker_detect", 10)
        self.timer = self.create_timer(0.033, self.process)
        self.fps_start_time = time.time()
        self.fps_frame_count = 0
        self.current_fps = 0.0
        self.last_log_time = time.time()
        threading.Thread(target=start_flask, daemon=True).start()
        self.get_logger().info("Inference engine started | Flask: http://0.0.0.0:5001/video")

    def rgb_cb(self, msg):
        self.latest_rgb = msg

    def depth_cb(self, msg):
        self.latest_depth = msg

    def calculate_distance(self, depth, cx, cy):
        h, w = depth.shape
        patch_size = 10
        x1 = max(0, cx - patch_size)
        x2 = min(w, cx + patch_size)
        y1 = max(0, cy - patch_size)
        y2 = min(h, cy + patch_size)
        patch = depth[y1:y2, x1:x2]
        if patch.size == 0:
            return None
        valid_values = patch[~np.isnan(patch)]
        if valid_values.size == 0:
            return None
        valid_values = valid_values[(valid_values > 0.1) & (valid_values < 20.0)]
        if valid_values.size == 0:
            return None
        return float(np.median(valid_values))

    def detect_color(self, roi):
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        ranges = {
            "orange": [(5, 120, 120), (20, 255, 255)],
            "red": [(0, 140, 120), (10, 255, 255)],
            "blue": [(90, 120, 120), (130, 255, 255)],
            "green": [(40, 120, 120), (80, 255, 255)],
            "yellow": [(22, 120, 120), (34, 255, 255)]
        }
        best, max_pix = None, 0
        for c, (lo, hi) in ranges.items():
            mask = cv2.inRange(hsv, tuple(lo), tuple(hi))
            cnt = cv2.countNonZero(mask)
            if cnt > max_pix:
                best, max_pix = c, cnt
        return best

    def nms_detections(self, detections, iou_threshold=0.5):
        if len(detections) == 0:
            return []
        detections = sorted(detections, key=lambda x: x[4], reverse=True)
        keep = []
        while len(detections) > 0:
            best = detections[0]
            keep.append(best)
            detections = detections[1:]
            filtered = []
            for det in detections:
                iou = self.calculate_iou(best[:4], det[:4])
                if iou < iou_threshold:
                    filtered.append(det)
            detections = filtered
        return keep
    
    def calculate_iou(self, box1, box2):
        x1_1, y1_1, x2_1, y2_1 = box1
        x1_2, y1_2, x2_2, y2_2 = box2
        xi1 = max(x1_1, x1_2)
        yi1 = max(y1_1, y1_2)
        xi2 = min(x2_1, x2_2)
        yi2 = min(y2_1, y2_2)
        inter_area = max(0, xi2 - xi1) * max(0, yi2 - yi1)
        box1_area = (x2_1 - x1_1) * (y2_1 - y1_1)
        box2_area = (x2_2 - x1_2) * (y2_2 - y1_2)
        union_area = box1_area + box2_area - inter_area
        if union_area == 0:
            return 0
        return inter_area / union_area

    def process(self):
        global latest_frame
        self.fps_frame_count += 1
        elapsed = time.time() - self.fps_start_time
        if elapsed > 1.0:
            self.current_fps = self.fps_frame_count / elapsed
            self.fps_frame_count = 0
            self.fps_start_time = time.time()

        if self.latest_rgb is None or self.latest_depth is None:
            return

        frame = self.bridge.imgmsg_to_cv2(self.latest_rgb, "bgr8")
        depth = self.bridge.imgmsg_to_cv2(self.latest_depth)
        if depth.dtype == np.uint16:
            depth = depth.astype(np.float32) * 0.001

        img = cv2.resize(frame, (640, 640))
        img = img.transpose(2, 0, 1)[None] / 255.0
        output = self.trt.infer(img)[0]

        h, w, _ = frame.shape
        scale_x = w / 640.0
        scale_y = h / 640.0
        img_center = w / 2

        all_detections = []
        for det in output.T:
            if det[4] < 0.50:
                continue
            cx = int(det[0] * scale_x)
            cy = int(det[1] * scale_y)
            bw = int(det[2] * scale_x)
            bh = int(det[3] * scale_y)
            x1 = max(0, cx - bw // 2)
            y1 = max(0, cy - bh // 2)
            x2 = min(w, cx + bw // 2)
            y2 = min(h, cy + bh // 2)
            all_detections.append((x1, y1, x2, y2, det[4], det))
        
        filtered_detections = self.nms_detections(all_detections, iou_threshold=0.5)
        published_count = 0

        for x1, y1, x2, y2, conf, det in filtered_detections:
            cx = (x1 + x2) // 2
            cy = (y1 + y2) // 2
            roi = frame[y1:y2, x1:x2]
            if roi.size == 0:
                continue

            color = self.detect_color(roi)
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
            published_count += 1

            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 255), 2)
            cv2.putText(frame, f"{color} {dist:.2f}m", (x1, y1 - 8), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)

        if time.time() - self.last_log_time > 5.0:
            self.get_logger().info(f"FPS: {self.current_fps:.1f} | Detections: {published_count}")
            self.last_log_time = time.time()

        cv2.putText(frame, f"FPS: {self.current_fps:.1f}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 0), 2)
        cv2.putText(frame, f"Detections: {len(filtered_detections)}", (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
        latest_frame = frame

def main():
    rclpy.init()
    node = InferenceEngine()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()
