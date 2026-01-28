import rclpy
from rclpy.node import Node
from sensor_msgs.msg import NavSatFix
import serial
import pynmea2

class GPSNode(Node):
    def __init__(self):
        super().__init__('gps_node')

        self.publisher_ = self.create_publisher(NavSatFix, '/fix', 10)

        port = '/dev/ttyACM0'
        baud = 115200

        try:
            self.ser = serial.Serial(port, baudrate=baud, timeout=0.1)
            self.get_logger().info(f"Connected to GPS on {port} at {baud} baud")
        except Exception as e:
            self.get_logger().error(f"Failed to open serial port: {e}")
            raise e

        # publish at 20 Hz (0.05 sec)
        self.timer = self.create_timer(0.05, self.read_gps)

    def read_gps(self):
        try:
            line = self.ser.readline().decode('ascii', errors='replace').strip()

            if not line:
                self.get_logger().warn("No data received from GPS")
                return

            # Log raw NMEA (debug level)
            self.get_logger().debug(f"NMEA: {line}")

            if line.startswith('$GNGGA') or line.startswith('$GPGGA'):
                msg = pynmea2.parse(line)

                fix = NavSatFix()
                fix.header.stamp = self.get_clock().now().to_msg()
                fix.header.frame_id = "gps_link"

                fix.latitude = msg.latitude
                fix.longitude = msg.longitude
                fix.altitude = float(msg.altitude) if msg.altitude else 0.0

                fix.status.status = 0
                fix.status.service = 1

                self.publisher_.publish(fix)

                # Log published data
                self.get_logger().info(
                    f"Published Fix → Lat: {fix.latitude:.6f}, "
                    f"Lon: {fix.longitude:.6f}, Alt: {fix.altitude:.2f}"
                )

        except pynmea2.ParseError as e:
            self.get_logger().warn(f"NMEA parse error: {e}")
        except Exception as e:
            self.get_logger().error(f"GPS read error: {e}")


def main(args=None):
    rclpy.init(args=args)
    node = GPSNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
