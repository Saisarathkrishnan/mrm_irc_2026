import rclpy
from rclpy.node import Node
from custom_msgs.msg import ImuData
from geometry_msgs.msg import Vector3
from custom_msgs.msg import ArmPwm
from std_msgs.msg import Bool

import socket
import time
import pickle
import os
import serial


class BNO055Node(Node):
    def __init__(self):
        super().__init__('rm_auto')

##### filler
        self.ip = "10.0.0.7"
        self.port = 5005
        self.sock = None
        self.server_addr = (self.ip, self.port)
        self.initialize_sock()
        self.msgR="L0R0T0U0E|Z1"
        self.msg="L0R0T0U0E|Z1"
        self.p=0
#####
        self.deliveryNow=False

        self.l1=dict({"r":400.0,"p":400.0,"y":400.0})
        self.l2=dict({"r":400.0,"p":400.0,"y":400.0})
        self.imu_data=dict({"r":400.0,"p":400.0,"y":400.0})
        self.toAnglel1=90
        self.toAnglel2=180

        self.link1Pwm=0
        self.link2Pwm=0
#        self.l2=dict()
        self.line="123"
        
        self.imu_pub_ = self.create_publisher(ImuData, '/imu_data', 10)

        self.delivery_sub_ = self.create_subscription(Bool, '/deliver_now',self.delivery_callback, 10)
        
        self.armPwm_ =self.create_publisher(ArmPwm,'/arm_pwm',10)
        
        self.delivered_pub_=self.create_publisher(Bool,"/delivered",10)

        self.initialize_imu()
        self.timer_ = self.create_timer(0.01, self.publish_imu)
#        self.timer_ = self.create_timer(0.01, self.send)

    def initialize_imu(self):
        self.serial_port = serial.Serial('/dev/ttyUSB0', 115200)
        time.sleep(1)
        if(self.serial_port):
            print("into the serial port1 saf asf")
        else:
            print("serial port issue")
        self.serial_port.dtr = False   # GPIO0 HIGH
        self.serial_port.rts = True    # EN LOW (reset)
        time.sleep(0.1)

        self.serial_port.rts = False   # EN HIGH (run)
        time.sleep(0.1)
        self.serial_port.dtr = True 
        self.serial_port.close()
        self.serial_port = serial.Serial('/dev/ttyUSB0', 115200)
        time.sleep(1)
        if(self.serial_port):
            print("into the serial port reseted")
        else:
            print("serial port issue")

    def delivery_callback(self,msg):
        if(msg.data):
            self.deliveryNow=True
            print(msg.data)
        else:
            self.deliveryNow=False
            print(msg.data)
    def initialize_sock(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            print("UDP socket initialized")
        except socket.error as e:
            print("Socket error:", e)
            self.sock = None
        
    def send(self):
        print(self.msg)
        if self.sock is None:
            print("Socket not initialized")
            return

        try:
            data = self.msg.encode("utf-8")
            self.sock.sendto(data, self.server_addr)
        except socket.error as e:
            print("Send error:", e)

    def publish_imu(self):
        print("hi")
        self.line = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
        x=self.line
        if(len(self.line)!=0):
            print(self.line)

        if(len(self.line)!=0 and str(self.line[0])=="y"):
            print(self.line)

            y=type(self.line)
            z=x.split('_')
            self.l1["r"]=float(z[2])
            self.l1["p"]=-float(z[3])
            self.l1["y"]=float(z[1])

            self.l2["r"]=float(z[6])
            self.l2["p"]=float(z[7])
            self.l2["y"]=float(z[5])

            self.l1["p"]=self.l1["p"] % 360
            self.l2["p"]=self.l2["p"] % 360

            print("l1->",self.l1['p'],"___l2->",self.l2['p'])
            
            
            if(self.deliveryNow):
                link1Diff=self.toAnglel1-self.l1["p"]
                link2Diff=self.toAnglel2-self.l2["p"]

                if(abs(link1Diff)<2 and abs(link2Diff)<2):
                    self.deliveryNow=False
                    msgggg=Bool()
                    msgggg.data=True
                    self.delivered_pub_.publish(msgggg)
                    return
                
                if(abs(link1Diff) > 2):
                    if(link1Diff>0):
                        #print("link1 diff 1" ,link1Diff)
                        self.link1Pwm= -1
                    if(link1Diff<0):
                        #print("link1 diff -1" ,link1Diff)
                        self.link1Pwm= 1 
                else:
                    self.link1Pwm=0
                
                if(abs(link2Diff) > 2):
                    if(link2Diff>0):
                        self.link2Pwm=-1
                    if(link2Diff<0):
                        self.link2Pwm=1 
                else:
                    self.link2Pwm=0
                    self.link2Pwm=0
                

                result=self.msgR
                rm_values=ArmPwm()
                print("llll",self.link1Pwm,self.link2Pwm)
                rm_values.link1=self.link1Pwm
                rm_values.link2=self.link2Pwm

                self.armPwm_.publish(rm_values)
                
                #rm_values.gripper=self.link2Pwm
            else:
                self.armPwm_.publish(ArmPwm())

            
            #result=self.msgR.replace("T0","T"+str(self.link1Pwm))
            #result=result.replace("U0","U"+str(self.link2Pwm))
            #
            #print("lll",self.link1Pwm,result)
            #
#
            #
            #self.msg=self.msgR
            #self.msg=result
#
            #self.send()





            

'''
        if(self.line[0]!="2"):
            self.initialize_imu()
        else: 
            data=self.line[2:-1]
            xcv=data.split("_")
            for i in xcv:
                xcv[xcv.index(i)]=float(i)
            
            
            self.yaw=xcv[0]
            self.pitch=xcv[1]
            self.roll=xcv[2]
            print(f"r: {self.roll:7.2f} p: {self.pitch:7.2f} y: {self.yaw:7.2f}", end='\r')

            imu_msg_ = ImuData()
            imu_msg_.orientation.x = self.roll
            imu_msg_.orientation.y = self.pitch
            imu_msg_.orientation.z = self.yaw

            imu_msg_.acceleration.x = 0.0
            imu_msg_.acceleration.y = 0.0
            imu_msg_.acceleration.z = 0.0

            # Publish
            self.imu_pub_.publish(imu_msg_)
'''


def main(args=None):
    rclpy.init(args=args)
    node = BNO055Node()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.save_calibration()
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
