#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
import serial
import pynmea2

from sensor_msgs.msg import NavSatFix, NavSatStatus


class GPSNode(Node):
    def __init__(self):
        super().__init__("gps_node")

        # Parameters
        self.declare_parameter("port", "/dev/ttyACM0")
        self.declare_parameter("baud", 9600)
        self.declare_parameter("frame_id", "gps")

        port = self.get_parameter("port").value
        baud = self.get_parameter("baud").value
        self.frame_id = self.get_parameter("frame_id").value

        # Publisher
        self.pub = self.create_publisher(NavSatFix, "/fix", 10)

        # Serial
        self.ser = serial.Serial(port, baud, timeout=1)

        # Timer
        self.timer = self.create_timer(0.1, self.read_gps)

        self.get_logger().info(f"GPS started on {port} @ {baud} baud")

    def read_gps(self):
        try:
            line = self.ser.readline().decode("ascii", errors="ignore").strip()

            if not line.startswith("$"):
                return

            msg = pynmea2.parse(line)

            if isinstance(msg, pynmea2.types.talker.GGA):
                fix = NavSatFix()
                fix.header.stamp = self.get_clock().now().to_msg()
                fix.header.frame_id = self.frame_id

                fix.latitude = msg.latitude
                fix.longitude = msg.longitude
                fix.altitude = msg.altitude if msg.altitude else 0.0

                fix.status.status = (
                    NavSatStatus.STATUS_FIX
                    if int(msg.gps_qual) > 0
                    else NavSatStatus.STATUS_NO_FIX
                )
                fix.status.service = NavSatStatus.SERVICE_GPS

                self.pub.publish(fix)

        except Exception:
            pass


def main():
    rclpy.init()
    node = GPSNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
