#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
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
                time.sleep(0.05)
                continue
            ok, jpg = cv2.imencode(
                ".jpg",
                latest_frame,
                [cv2.IMWRITE_JPEG_QUALITY, 70]
            )
            if ok:
                yield (b"--frame\r\n"
                       b"Content-Type: image/jpeg\r\n\r\n" +
                       jpg.tobytes() + b"\r\n")
    return Response(gen(), mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0", port=5001, debug=False, threaded=True)

class Viewer(Node):
    def __init__(self):
        super().__init__("viewer")
        self.bridge = CvBridge()
        self.create_subscription(
            Image,
            "/zed/zed_node/rgb/image_rect_color",
            self.cb,
            5
        )
        threading.Thread(target=start_flask, daemon=True).start()
        self.get_logger().info("Viewer started | http://<IP>:5001/video")

    def cb(self, msg):
        global latest_frame
        latest_frame = self.bridge.imgmsg_to_cv2(msg, "bgr8")

def main():
    rclpy.init()
    rclpy.spin(Viewer())
    rclpy.shutdown()

if __name__ == "__main__":
    main()
