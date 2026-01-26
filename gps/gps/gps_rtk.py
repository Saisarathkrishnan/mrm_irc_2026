import rclpy
from rclpy.node import Node
from sensor_msgs.msg import NavSatFix, NavSatStatus
import serial
import pynmea2

class GPSNode(Node):
    def __init__(self):
        super().__init__('gps_node')

        self.publisher = self.create_publisher(NavSatFix, '/fix', 10)

        # Change this to your GPS port
        self.serial_port = serial.Serial('/dev/ttyACM1', 9600, timeout=1)

        self.timer = self.create_timer(0.2, self.read_gps)

    def read_gps(self):
        line = self.serial_port.readline().decode('ascii', errors='replace')

        if line.startswith('$GPGGA') or line.startswith('$GNGGA'):
            try:
                msg = pynmea2.parse(line)

                fix = NavSatFix()
                fix.header.stamp = self.get_clock().now().to_msg()
                fix.header.frame_id = "gps_link"

                fix.status.status = NavSatStatus.STATUS_FIX
                fix.status.service = NavSatStatus.SERVICE_GPS

                fix.latitude = msg.latitude
                fix.longitude = msg.longitude
                fix.altitude = float(msg.altitude)

                fix.position_covariance_type = NavSatFix.COVARIANCE_TYPE_UNKNOWN

                self.publisher.publish(fix)

                self.get_logger().info(
                    f"Published: lat={fix.latitude}, lon={fix.longitude}, alt={fix.altitude}"
                )

            except Exception as e:
                self.get_logger().warn(f"GPS parse error: {e}")

def main():
    rclpy.init()
    node = GPSNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
