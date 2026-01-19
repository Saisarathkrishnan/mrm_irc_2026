#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
from ultralytics import YOLO
from PIL import Image as PILImage
import numpy as np
from custom_msgs.msg import MarkerTag
from flask import Flask, Response
import threading
import time
import torch

app = Flask(__name__)
node_instance = None


class YOLOColorInference(Node):
    def __init__(self):
        super().__init__("inference")

        self.bridge = CvBridge()
        self.model = YOLO("/home/mrmnavjet/IRC2026/ircWS/src/yolo/yolo/cone_v1.pt")

        self.latest_image = None
        self.latest_depth = None
        self.frame = None
        self.running_inference = False

        self.COLOR_ID = {
            "orange": 1,
            "red": 2,
            "blue": 3,
            "green": 4,
            "yellow": 5
        }

        self.image_sub = self.create_subscription(
            Image, "/zed/zed_node/rgb/image_rect_color", self.image_callback, 10
        )

        self.depth_sub = self.create_subscription(
            Image, "/zed/zed_node/depth/depth_registered", self.depth_callback, 10
        )

        self.marker_pub = self.create_publisher(MarkerTag, "/marker_detect", 10)
        self.timer = self.create_timer(0.05, self.process_frame)

    def image_callback(self, msg):
        self.latest_image = msg

    def depth_callback(self, msg):
        self.latest_depth = msg

    def calculate_distance(self, depth_map, cx, cy):
        patch = depth_map[cy - 5:cy + 5, cx - 5:cx + 5]
        if patch.size == 0:
            return None
        d = np.nanmean(patch)
        if np.isnan(d) or d <= 0:
            return None
        return float(d)

    def detect_color(self, roi):
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        color_ranges = {
            "orange": [(5, 120, 120), (20, 255, 255)],
            "red": [(0, 140, 120), (10, 255, 255)],
            "blue": [(90, 120, 120), (130, 255, 255)],
            "green": [(40, 120, 120), (80, 255, 255)],
            "yellow": [(22, 120, 120), (34, 255, 255)]
        }
        best = None
        max_pix = 0
        for c, (lo, hi) in color_ranges.items():
            mask = cv2.inRange(hsv, tuple(lo), tuple(hi))
            cnt = cv2.countNonZero(mask)
            if cnt > max_pix:
                best = c
                max_pix = cnt
        return best

    def process_frame(self):
        if self.running_inference:
            return
        if self.latest_image is None or self.latest_depth is None:
            return

        self.running_inference = True
        try:
            frame = self.bridge.imgmsg_to_cv2(self.latest_image, "bgr8")
            depth_map = self.bridge.imgmsg_to_cv2(self.latest_depth, "32FC1")

            device = 'cuda' if torch.cuda.is_available() else 'cpu'
            results = self.model.predict(
                frame,
                device=device,
                half=(device == 'cuda'),
                verbose=False
            )[0]

            h, w, _ = frame.shape
            img_center = w / 2

            for det in results.boxes:
                x1, y1, x2, y2 = map(int, det.xyxy[0].cpu())
                roi = frame[y1:y2, x1:x2]
                if roi.size == 0:
                    continue

                color_name = self.detect_color(roi)
                if color_name is None:
                    continue

                cx = int((x1 + x2) / 2)
                cy = int((y1 + y2) / 2)

                dist = self.calculate_distance(depth_map, cx, cy)
                if dist is None:
                    dist = 100.0

                y_offset = (cx - img_center) / 558.0

                msg = MarkerTag()
                msg.is_found = True
                msg.id = self.COLOR_ID[color_name]
                msg.x = dist
                msg.y = -y_offset
                self.marker_pub.publish(msg)

                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 255), 2)
                cv2.putText(frame, f"{color_name} {dist:.2f}m",
                            (x1, y1 - 10),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.6, (0, 255, 255), 2)

            self.frame = frame

        except Exception as e:
            print("Processing error:", e)

        finally:
            self.running_inference = False


def generate_frames():
    global node_instance
    while True:
        if node_instance and node_instance.frame is not None:
            ok, buffer = cv2.imencode('.jpg', node_instance.frame)
            if ok:
                frame = buffer.tobytes()
                yield (b'--frame\r\n'
                       b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
        time.sleep(0.03)


@app.route('/')
def index():
    return "Visit /video for YOLO stream"


@app.route('/video')
def video():
    return Response(generate_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


def flask_thread():
    app.run(host="0.0.0.0", port=5001, debug=False, use_reloader=False, threaded=True)


def main(args=None):
    global node_instance
    rclpy.init(args=args)

    node_instance = YOLOColorInference()

    t = threading.Thread(target=flask_thread)
    t.daemon = True
    t.start()

    try:
        rclpy.spin(node_instance)
    except KeyboardInterrupt:
        pass

    node_instance.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
