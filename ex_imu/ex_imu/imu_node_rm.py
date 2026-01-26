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
def safe_float(val, default=None):
    try:
        return float(val)
    except (ValueError, TypeError):
        return default

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
        self.rvYaw=400
        self.rvRoll=400
        self.rvPitch=400


        self.link1Pwm=0
        self.link2Pwm=0
#        self.l2=dict()
        self.line="123"
        
        self.imu_pub_ = self.create_publisher(ImuData, '/external_imu', 10)

        self.delivery_sub_ = self.create_subscription(Bool, '/deliver_now',self.delivery_callback, 10)
        
        self.armPwm_ =self.create_publisher(ArmPwm,'/arm_pwm',10)
        
        self.delivered_pub_=self.create_publisher(Bool,"/delivered",10)

        self.initialize_imu()
        self.timer_ = self.create_timer(0.01, self.publish_imu)
#        self.timer_ = self.create_timer(0.01, self.send)

    def initialize_imu(self):
        #self.serial_port = serial.Serial('/dev/ttyUSB0', 115200)
        self.serial_port = serial.Serial(
    '/dev/ttyUSB0',
    115200,
    timeout=0.001   # NON-BLOCKING
)

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
        try:
            
            line = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
            if not line:
                return

            if not line.startswith("yl1"):
                print(line)
                return

            z = line.split('_')
            if len(z) != 12:
                print("Malformed IMU packet:", line)
                return

            # Link 1
            self.l1["y"] = safe_float(z[1])
            self.l1["r"] = safe_float(z[2])
            self.l1["p"] = (-safe_float(z[3])) % 360

            # Link 2
            self.l2["y"] = safe_float(z[5])
            self.l2["r"] = safe_float(z[6])
            self.l2["p"] = safe_float(z[7]) % 360

            # Rotation Vector (Euler from ESP)
            self.rvYaw   = safe_float(z[9])
            self.rvRoll  = safe_float(z[10])
            self.rvPitch = safe_float(z[11])

            imuD = ImuData()
            imuD.orientation.z = self.rvYaw
            imuD.orientation.x = self.rvRoll
            imuD.orientation.y = self.rvPitch
            print(
    f"L1 [Y:{self.l1['y']:.2f} R:{self.l1['r']:.2f} P:{self.l1['p']:.2f}] | "
    f"L2 [Y:{self.l2['y']:.2f} R:{self.l2['r']:.2f} P:{self.l2['p']:.2f}] | "
    f"RV [Yaw:{self.rvYaw:.2f} Pitch:{self.rvPitch:.2f} Roll:{self.rvRoll:.2f}]"
)

            self.imu_pub_.publish(imuD)

        except Exception as e:
            print("IMU parse error:", e)


            
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
