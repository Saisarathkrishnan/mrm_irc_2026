#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import NavSatFix, NavSatStatus
import serial
import pynmea2
import time

class GPSNode(Node):
    def __init__(self):
        super().__init__('gps_node')

        self.declare_parameter('port', '/dev/ttyACM0')
        self.declare_parameter('baudrate', 115200)

        self.port = self.get_parameter('port').value
        self.baudrate = self.get_parameter('baudrate').value

        self.pub = self.create_publisher(NavSatFix, '/fix', 10)

        self.ser = None
        self._open_serial()

        self.last_raw_log = 0.0
        self.last_fix_log = 0.0

        self.timer = self.create_timer(0.1, self.read_gps)

    def _open_serial(self):
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            self.get_logger().info(f"[GPS] Opened port {self.port} @ {self.baudrate}")
        except Exception as e:
            self.ser = None
            self.get_logger().error(f"[GPS] Port open failed: {e}")

    def read_gps(self):
        if self.ser is None:
            self._open_serial()
            time.sleep(1.0)
            return

        try:
            line = self.ser.readline().decode('ascii', errors='ignore').strip()
            if not line:
                return

            now = time.time()

            # Raw NMEA visibility (2 Hz)
            if now - self.last_raw_log > 0.5:
                self.get_logger().info(f"[GPS RAW] {line}")
                self.last_raw_log = now

            if not line.startswith('$') or 'GGA' not in line:
                return

            msg = pynmea2.parse(line)
            fix_quality = int(msg.gps_qual)

            if fix_quality == 0:
                return

            fix = NavSatFix()
            fix.header.stamp = self.get_clock().now().to_msg()
            fix.header.frame_id = 'gps_link'

            fix.status.status = NavSatStatus.STATUS_FIX
            fix.status.service = NavSatStatus.SERVICE_GPS

            fix.latitude = msg.latitude
            fix.longitude = msg.longitude
            fix.altitude = float(msg.altitude)

            fix.position_covariance_type = NavSatFix.COVARIANCE_TYPE_UNKNOWN

            self.pub.publish(fix)

            # Fix log (0.5 Hz)
            if now - self.last_fix_log > 2.0:
                self.get_logger().info(
                    f"[GPS FIX] q={fix_quality} "
                    f"lat={fix.latitude:.7f} "
                    f"lon={fix.longitude:.7f} "
                    f"alt={fix.altitude:.2f}"
                )
                self.last_fix_log = now

        except pynmea2.ParseError:
            pass
        except serial.SerialException:
            self.get_logger().error("[GPS] Serial disconnected")
            self.ser = None
        except Exception as e:
            self.get_logger().error(f"[GPS] Read error: {e}")

def main():
    rclpy.init()
    node = GPSNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
