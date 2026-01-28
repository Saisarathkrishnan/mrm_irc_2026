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
CONF_THRES=0.6

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

@app.route("/video")
def video():
    def gen():
        global latest_frame
        while True:
            if latest_frame is None:
                time.sleep(0.01)
                continue
            f=cv2.resize(latest_frame,(0,0),fx=0.4,fy=0.4)
            ok,jpg=cv2.imencode(".jpg",f,[cv2.IMWRITE_JPEG_QUALITY,30])
            if not ok:
                continue
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n"+jpg.tobytes()+b"\r\n"
    return Response(gen(),mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0",port=5000,debug=False,threaded=True)

class ConeDetector(Node):
    def __init__(self):
        super().__init__("inference_compressed")
        self.bridge=CvBridge()
        self.model=YOLO(MODEL_PATH)
        self.depth_img=None
        self.rgb_sub=self.create_subscription(Image,RGB_TOPIC,self.rgb_cb,10)
        self.depth_sub=self.create_subscription(Image,DEPTH_TOPIC,self.depth_cb,10)
        self.pub=self.create_publisher(MarkerTag,PUBLISH_TOPIC,10)
        threading.Thread(target=start_flask,daemon=True).start()

    def depth_cb(self,msg):
        try:
            self.depth_img=self.bridge.imgmsg_to_cv2(msg,"passthrough")
        except:
            self.depth_img=None

    def classify_color(self,roi):
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
        if 0<=cx<w and 0<=cy<h:
            d=self.depth_img[int(cy),int(cx)]
            if np.isfinite(d) and d>0:
                return float(d)
        return -1.0

    def rgb_cb(self,msg):
        global latest_frame
        frame=self.bridge.imgmsg_to_cv2(msg,"bgr8")
        h,w=frame.shape[:2]
        res=self.model(frame,conf=CONF_THRES,verbose=False)[0]
        best_per_color={}
        for b in res.boxes:
            conf=float(b.conf[0])
            x1,y1,x2,y2=map(int,b.xyxy[0])
            cx,cy=(x1+x2)//2,(y1+y2)//2
            roi=frame[y1:y2,x1:x2]
            if roi.size==0:
                continue
            color=self.classify_color(roi)
            depth=self.get_depth(cx,cy)
            offset=(cx-w/2)/(w/2)
            if color not in best_per_color or conf>best_per_color[color][0]:
                best_per_color[color]=(conf,x1,y1,x2,y2,depth,offset)
        for color,data in best_per_color.items():
            conf,x1,y1,x2,y2,depth,offset=data
            m=MarkerTag()
            m.is_found=True
            m.id=COLOR_IDS[color]
            m.x=depth
            m.y=offset
            self.pub.publish(m)
            cv2.rectangle(frame,(x1,y1),(x2,y2),BOX_COLORS[color],2)
            cv2.putText(frame,f"{color} {conf:.2f}",(x1,y1-6),
                        cv2.FONT_HERSHEY_SIMPLEX,0.5,BOX_COLORS[color],1)
        if not best_per_color:
            m=MarkerTag()
            m.is_found=False
            m.id=COLOR_IDS["orange"]
            m.x=-1.0
            m.y=0.0
            self.pub.publish(m)
        latest_frame=frame

def main():
    rclpy.init()
    node=ConeDetector()
    signal.signal(signal.SIGINT,lambda s,f:(node.destroy_node(),rclpy.shutdown(),sys.exit(0)))
    rclpy.spin(node)

if __name__=="__main__":
    main()
