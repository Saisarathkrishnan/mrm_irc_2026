#!/usr/bin/env python3
import os
os.environ["YOLO_OFFLINE"]="1"
os.environ["ULTRALYTICS_OFFLINE"]="1"
os.environ["ULTRALYTICS_SETTINGS"]="False"

import rclpy
from rclpy.node import Node
import cv2
import numpy as np
from ultralytics import YOLO
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from custom_msgs.msg import MarkerTag
from flask import Flask, Response
import threading
import time
import signal
import sys

MODEL_PATH="/home/mrmnavjet/IRC2026/ircWS/mrm_irc_2026/yolo/yolo/cone_final.pt"
RGB_TOPIC="/zed/zed_node/rgb/color/rect/image"
DEPTH_TOPIC="/zed/zed_node/depth/depth_registered"
PUBLISH_TOPIC="/marker_detect"
CONF_THRES=0.85

INFER_W,INFER_H=640,384

COLOR_IDS={"orange":1,"red":2,"blue":3,"green":4,"yellow":5}
BOX_COLORS={
    "orange":(0,165,255),
    "red":(0,0,255),
    "blue":(255,0,0),
    "green":(0,255,0),
    "yellow":(0,255,255)
}

HSV_RANGES={
    "orange":[((5,100,100),(20,255,255))],
    "red":[((0,100,100),(10,255,255)),((160,100,100),(180,255,255))],
    "blue":[((100,150,50),(130,255,255))],
    "green":[((40,50,50),(80,255,255))],
    "yellow":[((20,100,100),(35,255,255))]
}

app=Flask(__name__)
latest_frame=None
last_stream=0.0

@app.route("/video")
def video():
    def gen():
        global latest_frame,last_stream
        while True:
            if latest_frame is None or time.time()-last_stream<0.05:
                time.sleep(0.01)
                continue
            ok,jpg=cv2.imencode(".jpg",latest_frame,[cv2.IMWRITE_JPEG_QUALITY,90])
            if not ok:
                continue
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n"+jpg.tobytes()+b"\r\n"
    return Response(gen(),mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0",port=5000,debug=False,threaded=True)

class ConeDetector(Node):
    def __init__(self):
        super().__init__("cone_detector")
        self.bridge=CvBridge()
        self.model=YOLO(MODEL_PATH)
        self.depth_img=None
        self.prev_found=False
        self.rgb_sub=self.create_subscription(Image,RGB_TOPIC,self.rgb_cb,10)
        self.depth_sub=self.create_subscription(Image,DEPTH_TOPIC,self.depth_cb,10)
        self.pub=self.create_publisher(MarkerTag,PUBLISH_TOPIC,10)
        threading.Thread(target=start_flask,daemon=True).start()

    def depth_cb(self,msg):
        try:
            self.depth_img=self.bridge.imgmsg_to_cv2(msg,"passthrough")
        except:
            self.depth_img=None

    def preprocess_roi(self,roi):
        lab=cv2.cvtColor(roi,cv2.COLOR_BGR2LAB)
        l,a,b=cv2.split(lab)
        l=cv2.createCLAHE(2.0,(8,8)).apply(l)
        roi=cv2.merge([l,a,b])
        return cv2.cvtColor(roi,cv2.COLOR_LAB2BGR)

    def classify_color(self,roi):
        roi=self.preprocess_roi(roi)
        hsv=cv2.cvtColor(roi,cv2.COLOR_BGR2HSV)
        best,count="orange",0
        for c,ranges in HSV_RANGES.items():
            total=0
            for lo,hi in ranges:
                total+=cv2.countNonZero(cv2.inRange(hsv,np.array(lo),np.array(hi)))
            if total>count:
                count,best=total,c
        return best

    def get_depth(self,cx,cy):
        if self.depth_img is None:
            return -1.0
        h,w=self.depth_img.shape[:2]
        if cx<2 or cy<2 or cx>=w-2 or cy>=h-2:
            return -1.0
        patch=self.depth_img[cy-2:cy+3,cx-2:cx+3]
        patch=patch[np.isfinite(patch)]
        return float(np.median(patch)) if patch.size else -1.0

    def rgb_cb(self,msg):
        global latest_frame,last_stream
        frame=self.bridge.imgmsg_to_cv2(msg,"bgr8")
        h,w=frame.shape[:2]

        small=cv2.resize(frame,(INFER_W,INFER_H))
        sx,sy=w/INFER_W,h/INFER_H

        res=self.model(small,conf=CONF_THRES,verbose=False)[0]

        best=None
        best_depth=1e9

        for b in res.boxes:
            x1,y1,x2,y2=map(int,b.xyxy[0])
            x1,y1,x2,y2=int(x1*sx),int(y1*sy),int(x2*sx),int(y2*sy)
            cx,cy=(x1+x2)//2,(y1+y2)//2
            roi=frame[y1:y2,x1:x2]
            if roi.size==0:
                continue
            depth=self.get_depth(cx,cy)
            if 0<depth<best_depth:
                best_depth=depth
                best=(x1,y1,x2,y2,cx,cy,roi)

        found=False
        if best:
            x1,y1,x2,y2,cx,cy,roi=best
            color=self.classify_color(roi)
            offset=(cx-w/2)/(w/2)

            m=MarkerTag()
            m.is_found=True
            m.id=COLOR_IDS[color]
            m.x=best_depth
            m.y=offset
            self.pub.publish(m)
            self.prev_found=True
            found=True

            cv2.rectangle(frame,(x1,y1),(x2,y2),BOX_COLORS[color],2)
            cv2.putText(frame,f"{color} {best_depth:.2f}m",(x1,y1-6),
                        cv2.FONT_HERSHEY_SIMPLEX,0.6,BOX_COLORS[color],2)

        if not found and self.prev_found:
            m=MarkerTag()
            m.is_found=False
            m.id=COLOR_IDS["orange"]
            m.x=-1.0
            m.y=0.0
            self.pub.publish(m)
            self.prev_found=False

        latest_frame=frame
        last_stream=time.time()

def main():
    rclpy.init()
    node=ConeDetector()
    signal.signal(signal.SIGINT,lambda s,f:(node.destroy_node(),rclpy.shutdown(),sys.exit(0)))
    rclpy.spin(node)

if __name__=="__main__":
    main()
