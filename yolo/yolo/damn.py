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
import time

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

        self.create_subscription(Image, "/zed/zed_node/rgb/image_rect_color", self.rgb_cb, 5)
        self.create_subscription(Image, "/zed/zed_node/depth/depth_registered", self.depth_cb, 5)

        self.pub = self.create_publisher(MarkerTag, "/marker_detect", 10)
        self.timer = self.create_timer(0.02, self.process)

        self.frames = 0
        self.start = time.time()
        self.fps = 0.0
        self.last_log = time.time()

        self.get_logger().info("Inference node started (NO FLASK)")

    def rgb_cb(self, msg):
        self.latest_rgb = msg

    def depth_cb(self, msg):
        self.latest_depth = msg

    def calculate_distance(self, depth, cx, cy):
        s = 8
        h, w = depth.shape
        x1, x2 = max(0, cx - s), min(w, cx + s)
        y1, y2 = max(0, cy - s), min(h, cy + s)
        patch = depth[y1:y2, x1:x2]
        patch = patch[np.isfinite(patch)]
        patch = patch[(patch > 0.1) & (patch < 20.0)]
        return float(np.median(patch)) if patch.size else None

    def detect_color_fast(self, frame, cx, cy):
        s = 5
        h, w, _ = frame.shape
        x1, x2 = max(0, cx - s), min(w, cx + s)
        y1, y2 = max(0, cy - s), min(h, cy + s)
        patch = frame[y1:y2, x1:x2]
        if patch.size == 0:
            return None
        hsv = cv2.cvtColor(patch, cv2.COLOR_BGR2HSV)
        h_mean, s_mean, v_mean = np.mean(hsv.reshape(-1, 3), axis=0)
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

    def process(self):
        if self.latest_rgb is None or self.latest_depth is None:
            return

        self.frames += 1
        now = time.time()
        if now - self.start >= 1.0:
            self.fps = self.frames / (now - self.start)
            self.frames = 0
            self.start = now

        frame = self.bridge.imgmsg_to_cv2(self.latest_rgb, "bgr8")
        depth = self.bridge.imgmsg_to_cv2(self.latest_depth)
        if depth.dtype == np.uint16:
            depth = depth.astype(np.float32) * 0.001

        img = cv2.resize(frame, (640, 640))
        img = img.transpose(2, 0, 1)[None] / 255.0

        output = self.trt.infer(img)[0]

        h, w, _ = frame.shape
        sx = w / 640.0
        center = w / 2.0

        best = None
        best_conf = 0.0
        for det in output.T:
            if det[4] < 0.5:
                continue
            if det[4] > best_conf:
                best = det
                best_conf = det[4]

        if best is None:
            return

        cx = int(best[0] * sx)
        cy = int(best[1] * (h / 640.0))

        color = self.detect_color_fast(frame, cx, cy)
        if color is None:
            return

        dist = self.calculate_distance(depth, cx, cy)
        if dist is None:
            return

        msg = MarkerTag()
        msg.is_found = True
        msg.id = self.COLOR_ID[color]
        msg.x = dist
        msg.y = -(cx - center) / 558.0
        self.pub.publish(msg)

        if now - self.last_log > 2.0:
            self.get_logger().info(f"[PERF] FPS: {self.fps:.1f}")
            self.last_log = now

def main():
    rclpy.init()
    rclpy.spin(InferenceEngine())
    rclpy.shutdown()

if __name__ == "__main__":
    main()
