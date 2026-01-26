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
            ok, jpg = cv2.imencode(".jpg", latest_frame, [cv2.IMWRITE_JPEG_QUALITY, 95])
            if not ok:
                continue
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + jpg.tobytes() + b"\r\n"
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
        super().__init__("cone_inference")
        self.bridge = CvBridge()
        self.trt = TRTInfer("/home/mrmnavjet/IRC2026/ircWS/mrm_irc_2026/yolo/yolo/cone_final.engine")
        self.latest_rgb = None
        self.latest_depth = None
        self.COLOR_ID = {"orange":1,"red":2,"blue":3,"green":4,"yellow":5}
        self.BOX_COLOR = {
            "orange":(0,165,255),
            "red":(0,0,255),
            "blue":(255,0,0),
            "green":(0,255,0),
            "yellow":(0,255,255)
        }
        self.sub_rgb = self.create_subscription(Image, "/zed/zed_node/rgb/color/rect/image", self.rgb_cb, 10)
        self.sub_depth = self.create_subscription(Image, "/zed/zed_node/depth/depth_registered", self.depth_cb, 10)
        self.pub = self.create_publisher(MarkerTag, "/marker_detect", 10)
        self.timer = self.create_timer(0.033, self.process)
        threading.Thread(target=start_flask, daemon=True).start()

    def rgb_cb(self, msg):
        self.latest_rgb = msg

    def depth_cb(self, msg):
        self.latest_depth = msg

    def detect_color(self, roi):
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        masks = {
            "orange": cv2.inRange(hsv, (8,140,120), (22,255,255)),
            "blue": cv2.inRange(hsv, (100,150,120), (125,255,255)),
            "green": cv2.inRange(hsv, (45,140,120), (75,255,255)),
            "yellow": cv2.inRange(hsv, (24,150,120), (35,255,255))
        }
        red1 = cv2.inRange(hsv, (0,150,120), (8,255,255))
        red2 = cv2.inRange(hsv, (170,150,120), (180,255,255))
        masks["red"] = cv2.bitwise_or(red1, red2)
        best, maxc = None, 0
        for c, m in masks.items():
            cnt = cv2.countNonZero(m)
            if cnt > maxc:
                best, maxc = c, cnt
        return best

    def process(self):
        global latest_frame
        if self.latest_rgb is None:
            return

        frame = self.bridge.imgmsg_to_cv2(self.latest_rgb, "bgr8")
        depth = None
        if self.latest_depth is not None:
            depth = self.bridge.imgmsg_to_cv2(self.latest_depth)
            if depth.dtype == np.uint16:
                depth = depth.astype(np.float32) * 0.001

        h, w, _ = frame.shape
        img = cv2.resize(frame, (640,640))
        img = img.transpose(2,0,1)[None] / 255.0
        output = self.trt.infer(img)[0]

        scale_x = w / 640.0
        scale_y = h / 640.0
        center_x = w / 2.0

        for det in output.T:
            conf = float(det[4])
            if conf < 0.4:
                continue

            cx = int(det[0] * scale_x)
            cy = int(det[1] * scale_y)
            bw = int(det[2] * scale_x)
            bh = int(det[3] * scale_y)

            x1 = max(0, cx - bw//2)
            y1 = max(0, cy - bh//2)
            x2 = min(w, cx + bw//2)
            y2 = min(h, cy + bh//2)

            roi = frame[y1:y2, x1:x2]
            if roi.size == 0:
                continue

            color = self.detect_color(roi)
            if color is None:
                continue

            dist = -1.0
            if depth is not None:
                px = depth[max(0,cy-5):min(h,cy+5), max(0,cx-5):min(w,cx+5)]
                v = px[(px>0.1)&(px<20.0)]
                if v.size > 0:
                    dist = float(np.median(v))

            msg = MarkerTag()
            msg.is_found = True
            msg.id = self.COLOR_ID[color]
            msg.x = dist
            msg.y = -((cx - center_x) / 558.0)
            self.pub.publish(msg)

            bc = self.BOX_COLOR[color]
            cv2.rectangle(frame, (x1,y1), (x2,y2), bc, 2)
            cv2.putText(frame, f"{color} {conf:.2f}", (x1, y1-6), cv2.FONT_HERSHEY_SIMPLEX, 0.7, bc, 2)

        latest_frame = frame

def main():
    rclpy.init()
    node = InferenceEngine()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()
