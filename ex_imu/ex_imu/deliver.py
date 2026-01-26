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

class ex_imu_lmao(Node):
    def __init__(self):
        super().__init__('rm_auto')

##### filler
        self.ip = "10.0.0.7"
        self.port = 5005
        self.sock = None
        self.server_addr = (self.ip, self.port)
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
        self.deliveryNow = False
        self.start_time = None
        self.arm_pwm=ArmPwm()
        self.imu_pub_ = self.create_publisher(ImuData, '/external_imu', 10)

        self.delivery_sub_ = self.create_subscription(Bool, '/deliver_now',self.delivery_callback, 10)
        
        self.armPwm_ =self.create_publisher(ArmPwm,'/arm_pwm',10)
        
        self.delivered_pub_=self.create_publisher(Bool,"/delivered",10)

        #self.initialize_imu()
        self.timer_ = self.create_timer(0.01, self.publish_imu)
#        self.timer_ = self.create_timer(0.01, self.send)


    def delivery_callback(self,msg):
        self.arm_pwm=ArmPwm()
        if(msg.data):
            self.deliveryNow=True
            print(msg.data)
        else:
            self.deliveryNow=False
            print(msg.data)

    def actuate_link2(self):
        self.get_logger().info("Link2 actuating")

    def actuate_gripper(self):
        self.get_logger().info("Gripper actuating")

    def stop_all(self):
        self.get_logger().info("Sequence complete")



    def publish_imu(self):
        self.arm_pwm=ArmPwm()
        if(self .deliveryNow):
            print("delivery started")
            self.deliveryNow=True
            if self.start_time is None:
                self.start_time = time.time()

            elapsed = time.time() - self.start_time

            if elapsed < 3.0:
                self.actuate_link2()
            
                self.arm_pwm.link2=-1
                
            elif elapsed < 6.0:
                self.arm_pwm.gripper= -1
            else:
                self.stop_all()
                self.deliveryNow = False
                self.start_time = None
                x=Bool()
                x.data=True
                self.delivered_pub_.publish(x)
        print(self.arm_pwm)
        self.armPwm_.publish(self.arm_pwm)


    
            



def main(args=None):
    rclpy.init(args=args)
    node = ex_imu_lmao()
    try:
        rclpy.spin(node)

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
