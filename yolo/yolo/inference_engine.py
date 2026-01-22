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
import threading, time

YOLO_CONF_THRESH = 0.50
NMS_IOU_THRESH = 0.45
ENGINE_PATH = "/home/mrmnavjet/IRC2026/ircWS/mrm_irc_2026/yolo/yolo/cone_v1.engine"

FX = 475.26

COLOR_ID = {
    "orange": 1,
    "red": 2,
    "blue": 3,
    "green": 4,
    "yellow": 5
}

BOX_COLORS = {
    "red": (0,0,255),
    "blue": (255,0,0),
    "orange": (0,165,255),
    "green": (0,255,0),
    "yellow": (0,255,255)
}

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
            f = cv2.resize(f,(640,360),interpolation=cv2.INTER_LINEAR)
            ok,jpg = cv2.imencode(".jpg",f,[cv2.IMWRITE_JPEG_QUALITY,25])
            if ok:
                yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n"+jpg.tobytes()+b"\r\n"
            time.sleep(0.12)
    return Response(gen(),mimetype="multipart/x-mixed-replace; boundary=frame")

def start_flask():
    app.run(host="0.0.0.0",port=5001,threaded=True)

class TRT:
    def __init__(self,path):
        logger = trt.Logger(trt.Logger.WARNING)
        with open(path,"rb") as f,trt.Runtime(logger) as rt:
            self.engine = rt.deserialize_cuda_engine(f.read())
        self.ctx = self.engine.create_execution_context()
        self.stream = cuda.Stream()
        self.in_name = self.engine.get_tensor_name(0)
        self.out_name = self.engine.get_tensor_name(1)
        self.din = cuda.mem_alloc(trt.volume(self.engine.get_tensor_shape(self.in_name))*4)
        self.dout = cuda.mem_alloc(trt.volume(self.engine.get_tensor_shape(self.out_name))*4)
        self.ctx.set_tensor_address(self.in_name,int(self.din))
        self.ctx.set_tensor_address(self.out_name,int(self.dout))
        self.hout = np.empty(self.engine.get_tensor_shape(self.out_name),np.float32)

    def infer(self,img):
        cuda.memcpy_htod_async(self.din,img,self.stream)
        self.ctx.execute_async_v3(self.stream.handle)
        cuda.memcpy_dtoh_async(self.hout,self.dout,self.stream)
        self.stream.synchronize()
        return self.hout

def iou(a,b):
    x1=max(a[0],b[0]); y1=max(a[1],b[1])
    x2=min(a[2],b[2]); y2=min(a[3],b[3])
    inter=max(0,x2-x1)*max(0,y2-y1)
    if inter<=0: return 0.0
    areaA=(a[2]-a[0])*(a[3]-a[1])
    areaB=(b[2]-b[0])*(b[3]-b[1])
    return inter/(areaA+areaB-inter)

def nms(dets):
    dets=sorted(dets,key=lambda x:x["conf"],reverse=True)
    keep=[]
    for d in dets:
        if all(iou(d["box"],k["box"])<NMS_IOU_THRESH for k in keep):
            keep.append(d)
    return keep

class NodeYOLO(Node):
    def __init__(self):
        super().__init__("inference_engine")
        self.bridge = CvBridge()
        self.trt = TRT(ENGINE_PATH)
        self.rgb = None
        self.depth = None
        self.input_buf = np.empty((1,3,640,640),np.float32)

        self.last_t = time.monotonic()
        self.fps = 0.0

        self.create_subscription(Image,"/zed/zed_node/rgb/color/rect/image",self.cb_rgb,10)
        self.create_subscription(Image,"/zed/zed_node/depth/depth_registered",self.cb_depth,10)
        self.pub = self.create_publisher(MarkerTag,"/marker_detect",10)

        self.create_timer(0.0,self.process)
        threading.Thread(target=start_flask,daemon=True).start()

    def cb_rgb(self,m): self.rgb = m
    def cb_depth(self,m): self.depth = m

    def detect_color(self,roi):
        hsv=cv2.cvtColor(roi,cv2.COLOR_BGR2HSV)
        masks={
            "red":(
                cv2.inRange(hsv,(0,110,70),(8,255,255)) |
                cv2.inRange(hsv,(170,110,70),(180,255,255))
            ),
            "orange":cv2.inRange(hsv,(8,130,90),(22,255,255)),
            "yellow":cv2.inRange(hsv,(22,120,90),(35,255,255)),
            "green":cv2.inRange(hsv,(36,90,80),(85,255,255)),
            "blue":cv2.inRange(hsv,(90,110,70),(135,255,255)),
        }
        total=roi.shape[0]*roi.shape[1]
        best=None; best_ratio=0.0
        for c,m in masks.items():
            r=cv2.countNonZero(m)/total
            if r>best_ratio:
                best_ratio=r; best=c
        return best if best_ratio>0.20 else None

    def process(self):
        global latest_frame
        if self.rgb is None: return

        t=time.monotonic()
        self.fps=0.9*self.fps+0.1*(1.0/max(1e-6,t-self.last_t))
        self.last_t=t

        frame=self.bridge.imgmsg_to_cv2(self.rgb,"bgr8")
        H,W,_=frame.shape

        img=cv2.resize(frame,(640,640))
        self.input_buf[0]=img.transpose(2,0,1)/255.0
        out=self.trt.infer(self.input_buf)[0]

        depth=None
        if self.depth:
            depth=self.bridge.imgmsg_to_cv2(self.depth,"passthrough")
            if depth.dtype==np.uint16:
                depth=depth.astype(np.float32)*0.001

        sx=W/640.0
        sy=H/640.0

        dets=[]

        for d in out.T:
            if d[4]<YOLO_CONF_THRESH: continue
            cx=int(d[0]*sx)
            cy=int(d[1]*sy)
            bw=int(d[2]*sx)
            bh=int(d[3]*sy)
            x1,y1,x2,y2=cx-bw//2,cy-bh//2,cx+bw//2,cy+bh//2
            if x1<0 or y1<0 or x2>=W or y2>=H: continue

            roi=frame[y1:y2,x1:x2]
            if roi.size==0: continue

            color=self.detect_color(roi)
            if not color: continue

            dist=float("nan")
            if depth is not None and 2<=cx<depth.shape[1]-2 and 2<=cy<depth.shape[0]-2:
                dwin=depth[cy-2:cy+2,cx-2:cx+2]
                v=dwin[(dwin>0.2)&(dwin<15.0)]
                if v.size>=4: dist=float(np.median(v))

            dets.append({"box":(x1,y1,x2,y2),"conf":float(d[4]),"color":color,"dist":dist,"cx":cx})

        dets=nms(dets)

        for d in dets:
            x1,y1,x2,y2=d["box"]; c=d["color"]; cx=d["cx"]
            cv2.rectangle(frame,(x1,y1),(x2,y2),BOX_COLORS[c],2)
            cv2.putText(frame,f"{c} {d['dist']:.2f}m",(x1,y1-4),
                        cv2.FONT_HERSHEY_SIMPLEX,0.5,BOX_COLORS[c],2)

            if not np.isnan(d["dist"]):
                msg=MarkerTag()
                msg.is_found=True
                msg.id=COLOR_ID[c]
                msg.x=float(d["dist"])
                msg.y=float((cx-(W*0.5))*d["dist"]/FX)
                self.pub.publish(msg)

        cv2.putText(frame,f"FPS: {self.fps:.1f}",(10,30),
                    cv2.FONT_HERSHEY_SIMPLEX,0.8,(255,255,255),2)

        with lock:
            latest_frame=frame

def main():
    rclpy.init()
    rclpy.spin(NodeYOLO())
    rclpy.shutdown()

if __name__=="__main__":
    main()
