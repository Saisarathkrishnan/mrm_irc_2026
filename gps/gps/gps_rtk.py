#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import NavSatFix, NavSatStatus
from serial import Serial, SerialException
from pyubx2 import UBXReader
import sys
import threading

FIX_MAP = {
    0: "NO FIX",
    1: "DEAD RECKONING",
    2: "2D FIX",
    3: "3D FIX",
    4: "GNSS + DR",
    5: "TIME ONLY"
}

class UbxParserNode(Node):
    def __init__(self):
        super().__init__('gps')

        self.declare_parameter('serial_port', '/dev/ttyACM0')
        self.declare_parameter('baud_rate', 38400)
        self.declare_parameter('topic_name', '/fix')

        serial_port = self.get_parameter('serial_port').value
        baud_rate = self.get_parameter('baud_rate').value
        topic_name = self.get_parameter('topic_name').value

        try:
            self.stream = Serial(serial_port, baud_rate, timeout=1)
            self.get_logger().info(f"Connected to GPS on {serial_port}")
        except SerialException as e:
            self.get_logger().fatal(f"GPS serial open failed: {e}")
            sys.exit(1)

        self.gps_pub = self.create_publisher(NavSatFix, topic_name, 10)

        self.reader_thread = threading.Thread(
            target=self.read_and_publish_loop,
            daemon=True
        )
        self.reader_thread.start()

    def read_and_publish_loop(self):
        ubr = UBXReader(self.stream)
        while rclpy.ok():
            try:
                _, msg = ubr.read()
                if msg is None or msg.identity != "NAV-PVT":
                    continue

                lat = msg.lat / 1e7
                lon = msg.lon / 1e7
                alt = msg.hMSL / 1000.0
                fix_type = msg.fixType
                fix_str = FIX_MAP.get(fix_type, "UNKNOWN")

                navsat = NavSatFix()
                navsat.header.stamp = self.get_clock().now().to_msg()
                navsat.header.frame_id = 'base_link'

                navsat.latitude = lat
                navsat.longitude = lon
                navsat.altitude = alt

                navsat.status.status = NavSatStatus.STATUS_FIX if fix_type >= 2 else NavSatStatus.STATUS_NO_FIX
                navsat.status.service = NavSatStatus.SERVICE_GPS
                navsat.position_covariance_type = NavSatFix.COVARIANCE_TYPE_UNKNOWN

                self.gps_pub.publish(navsat)

                self.get_logger().info(
                    f"Lat: {lat:.7f}, Lon: {lon:.7f}, Alt: {alt:.2f} m | Fix: {fix_str}"
                )

            except Exception as e:
                self.get_logger().error(f"GPS read error: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = UbxParserNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
