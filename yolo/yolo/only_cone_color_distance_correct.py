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
import time, threading

from flask import Flask, Response

# ---------------- CONFIG ----------------
YOLO_CONF_THRESH = 0.45
NMS_IOU_THRESH = 0.35
ENGINE_PATH = "/home/mrmnavjet/IRC2026/ircWS/src/yolo/yolo/cone_v1.engine"

FX = 475.26
CONE_REAL_HEIGHT_M = 0.23  # 23 cm

# ---------------- Flask ----------------
app = Flask(__name__)
latest_frame = None
lock = threading.Lock()

@app.route("/video")
def video():
    def gen():
        global latest_frame
        while True:
            with lock:
                f = latest_frame
            if f is None:
                time.sleep(0.05)
                continue
            f = cv2.resize(f, (640, 360))
            ok, jpg = cv2.imencode(".jpg", f, [cv2.IMWRITE_JPEG_QUALITY, 25])
            if ok:
                yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + jpg.tobytes() + b"\r\n"
            time.sleep(0.1)
    return Response(gen(), mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0", port=5001, threaded=True)

# ---------------- TensorRT ----------------
class TRT:
    def __init__(self, path):
        logger = trt.Logger(trt.Logger.WARNING)
        with open(path, "rb") as f, trt.Runtime(logger) as rt:
            self.engine = rt.deserialize_cuda_engine(f.read())
        self.ctx = self.engine.create_execution_context()
        self.stream = cuda.Stream()
        self.in_name = self.engine.get_tensor_name(0)
        self.out_name = self.engine.get_tensor_name(1)
        self.din = cuda.mem_alloc(trt.volume(self.engine.get_tensor_shape(self.in_name)) * 4)
        self.dout = cuda.mem_alloc(trt.volume(self.engine.get_tensor_shape(self.out_name)) * 4)
        self.ctx.set_tensor_address(self.in_name, int(self.din))
        self.ctx.set_tensor_address(self.out_name, int(self.dout))
        self.hout = np.empty(self.engine.get_tensor_shape(self.out_name), np.float32)

    def infer(self, img):
        cuda.memcpy_htod_async(self.din, img, self.stream)
        self.ctx.execute_async_v3(self.stream.handle)
        cuda.memcpy_dtoh_async(self.hout, self.dout, self.stream)
        self.stream.synchronize()
        return self.hout

# ---------------- Utils ----------------
def iou(a, b):
    x1, y1 = max(a[0], b[0]), max(a[1], b[1])
    x2, y2 = min(a[2], b[2]), min(a[3], b[3])
    inter = max(0, x2 - x1) * max(0, y2 - y1)
    if inter <= 0:
        return 0.0
    areaA = (a[2] - a[0]) * (a[3] - a[1])
    areaB = (b[2] - b[0]) * (b[3] - b[1])
    return inter / (areaA + areaB - inter)

def nms(dets):
    dets = sorted(dets, key=lambda x: x["conf"], reverse=True)
    keep = []
    for d in dets:
        if all(iou(d["box"], k["box"]) < NMS_IOU_THRESH for k in keep):
            keep.append(d)
    return keep

# ---------------- Color Classification ----------------
def classify_color(bgr_roi):
    hsv = cv2.cvtColor(bgr_roi, cv2.COLOR_BGR2HSV)
    h, s, v = hsv[:,:,0], hsv[:,:,1], hsv[:,:,2]

    mask = (s > 80) & (v > 80)
    if np.count_nonzero(mask) < 50:
        return "unknown"

    h_vals = h[mask]
    h_peak = int(np.argmax(np.bincount(h_vals, minlength=180)))

    if h_peak < 8 or h_peak > 170: return "red"
    if 8 <= h_peak < 18: return "orange"
    if 18 <= h_peak < 32: return "yellow"
    if 32 <= h_peak < 85: return "green"
    if 85 <= h_peak < 135: return "blue"
    return "unknown"

COLOR_ID = {
    "orange": 1,
    "red": 2,
    "blue": 3,
    "green": 4,
    "yellow": 5,
    "unknown": 0
}

BOX_COLOR = {
    "orange": (0,165,255),
    "red": (0,0,255),
    "blue": (255,0,0),
    "green": (0,255,0),
    "yellow": (0,255,255),
    "unknown": (200,200,200)
}

# ---------------- ROS Node ----------------
class YOLOConeNode(Node):
    def __init__(self):
        super().__init__("yolo_cone_node")
        self.bridge = CvBridge()
        self.trt = TRT(ENGINE_PATH)
        self.rgb = None
        self.input_buf = np.empty((1, 3, 640, 640), np.float32)

        self.create_subscription(Image, "/zed/zed_node/rgb/color/rect/image", self.cb_rgb, 10)
        self.pub = self.create_publisher(MarkerTag, "/marker_topic", 10)

        self.create_timer(0.0, self.process)
        threading.Thread(target=start_flask, daemon=True).start()

    def cb_rgb(self, msg):
        self.rgb = msg

    def process(self):
        global latest_frame
        if self.rgb is None:
            return

        frame = self.bridge.imgmsg_to_cv2(self.rgb, "bgr8")
        H, W, _ = frame.shape

        img = cv2.resize(frame, (640, 640))
        self.input_buf[0] = img.transpose(2,0,1) / 255.0
        out = self.trt.infer(self.input_buf)[0]

        dets = []
        sx, sy = W / 640.0, H / 640.0

        for d in out.T:
            if d[4] < YOLO_CONF_THRESH:
                continue

            cx, cy = int(d[0]*sx), int(d[1]*sy)
            bw, bh = int(d[2]*sx), int(d[3]*sy)
            x1, y1 = cx - bw//2, cy - bh//2
            x2, y2 = cx + bw//2, cy + bh//2
            if x1 < 0 or y1 < 0 or x2 >= W or y2 >= H:
                continue

            dets.append({
                "box": (x1,y1,x2,y2),
                "conf": float(d[4]),
                "cx": cx,
                "bh": bh
            })

        dets = nms(dets)

        for d in dets:
            x1,y1,x2,y2 = d["box"]
            roi = frame[y1:y2, x1:x2]
            if roi.size == 0:
                continue

            color = classify_color(roi)
            if color == "unknown":
                continue

            box_h = max(1, d["bh"])
            x_dist = (FX * CONE_REAL_HEIGHT_M) / box_h
            y_dist = (d["cx"] - W*0.5) * x_dist / FX  # ✅ FINAL, CORRECT

            cv2.rectangle(frame, (x1,y1), (x2,y2), BOX_COLOR[color], 2)
            cv2.putText(
                frame,
                f"{color} x:{x_dist:.2f}m y:{y_dist:.2f}m",
                (x1, max(0,y1-6)),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.55,
                BOX_COLOR[color],
                2
            )

            msg = MarkerTag()
            msg.is_found = True
            msg.id = COLOR_ID[color]
            msg.x = float(x_dist)
            msg.y = float(y_dist)
            self.pub.publish(msg)

        with lock:
            latest_frame = frame

# ---------------- Main ----------------
def main():
    rclpy.init()
    rclpy.spin(YOLOConeNode())
    rclpy.shutdown()

if __name__ == "__main__":
    main()
