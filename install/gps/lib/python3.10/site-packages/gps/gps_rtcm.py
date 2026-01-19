# GPS + RTK
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import NavSatFix
from serial import Serial
from pyubx2 import UBXReader
import sys
import socket
import threading

# Initialize the socket connection
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
port = 12345
s.connect(('192.168.245.71', port))


class UbxParserNode(Node):
    def __init__(self):
        super().__init__('gps')

        # Declare and get parameters
        self.declare_parameter('serial_port', '/dev/ttyTHS0')
        self.declare_parameter('baud_rate', 38400)
        self.declare_parameter('rtcm_port', '10.0.0.101')
        self.declare_parameter('topic_name', '/fix')

        serial_port = self.get_parameter('serial_port').get_parameter_value().string_value
        baud_rate = self.get_parameter('baud_rate').get_parameter_value().integer_value
        topic_name = self.get_parameter('topic_name').get_parameter_value().string_value

        # Initialize serial connection
        try:
            self.stream = Serial(serial_port, baud_rate, timeout=1)
            self.get_logger().info(f"Connected to GPS on {serial_port}")
        except serial.SerialException as e:
            self.get_logger().error(f"Failed to connect to GPS on {serial_port}: {e}")
            sys.exit(1)

        # Create the GPS publisher
        self.gps_pub = self.create_publisher(NavSatFix, topic_name, 10)

    def sending_rtcm(self):
        while rclpy.ok():
            try:
                # Receive RTCM data from base station and send to GPS module
                received_rtcm = s.recv(1024)
                if not received_rtcm:
                    break
                self.get_logger().info("Receiving RTCM data")
                self.stream.write(received_rtcm)
            except Exception as e:
                self.get_logger().error(f"Error while sending RTCM: {e}")
                break

    def receiver_gps(self):
        ubr = UBXReader(self.stream)
        while rclpy.ok():
            try:
                (raw_data, parsed_data) = ubr.read()
                if parsed_data.identity == "NAV-PVT":
                    # Extract GPS data
                    fix_type, lat, lon, alt = parsed_data.fixType, parsed_data.lat, parsed_data.lon, parsed_data.hMSL
                    self.get_logger().info(f"FixType={fix_type}, Lat={lat}, Lon={lon}, Alt={alt/1000} m")

                    # Create and publish NavSatFix message
                    msg = NavSatFix()
                    msg.latitude = lat / 1e7  # Convert to decimal degrees
                    msg.longitude = lon / 1e7  # Convert to decimal degrees
                    msg.altitude = alt / 1000  # Convert mm to meters
                    msg.position_covariance_type = NavSatFix.COVARIANCE_TYPE_APPROXIMATED
                    self.gps_pub.publish(msg)

            except Exception as e:
                self.get_logger().error(f"Errrr while receiving GPS data: {e}")
                


def main(args=None):
    rclpy.init(args=args)

    # Create and start the UBX parser node
    ubx_node = UbxParserNode()

    # Start RTCM and GPS threads
    try:
        t1 = threading.Thread(target=ubx_node.sending_rtcm)
        t2 = threading.Thread(target=ubx_node.receiver_gps)
        t1.start()
        t2.start()
        t1.join()
        t2.join()
    except KeyboardInterrupt:
        pass

    ubx_node.destroy_node()
    rclpy.shutdown()
    sys.exit(0)


if __name__ == '__main__':
    main()
