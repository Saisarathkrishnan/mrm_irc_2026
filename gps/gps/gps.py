# Only GPS
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import NavSatFix
from serial import Serial, SerialException
from pyubx2 import UBXReader
import sys
import threading
import time

class UbxParserNode(Node):
    def __init__(self):
        super().__init__('gps')

        # Declare and get parameters
        self.declare_parameter('serial_port', '/dev/ttyACM0')
        self.declare_parameter('baud_rate', 38400)
        self.declare_parameter('topic_name', '/fix')

        serial_port = self.get_parameter('serial_port').get_parameter_value().string_value
        baud_rate = self.get_parameter('baud_rate').get_parameter_value().integer_value
        topic_name = self.get_parameter('topic_name').get_parameter_value().string_value

        # Initialize serial connection
        try:
            self.stream = Serial(serial_port, baud_rate, timeout=1)
            self.get_logger().info(f"Connected to GPS on {serial_port}")
        except SerialException as e:
            self.get_logger().error(f"Failed to connect to GPS on {serial_port}: {e}")
            sys.exit(1)

        # Create the GPS publisher
        self.gps_pub = self.create_publisher(NavSatFix, topic_name, 10)

        # Store last known GPS data
        self.last_lat = None
        self.last_lon = None
        self.last_alt = None

        # Create threads for reading GPS data and publishing it
        self.reading_thread = threading.Thread(target=self.read_gps_data_thread, daemon=True)
        self.publishing_thread = threading.Thread(target=self.publish_gps_data_thread, daemon=True)

        # Start both threads
        self.reading_thread.start()
        self.publishing_thread.start()

    def read_gps_data_thread(self):
        ubr = UBXReader(self.stream)
        while True:
            try:
                (raw_data, parsed_data) = ubr.read()
                if parsed_data.identity == "NAV-PVT":
                    # Extract GPS data
                    fix_type = parsed_data.fixType
                    lat = parsed_data.lat
                    lon = parsed_data.lon
                    alt = parsed_data.hMSL

                    # Log the GPS data
                    self.get_logger().info(f"FixType={fix_type}, Lat={lat}, Lon={lon}, Alt={alt / 1000} m")

                    # Update the last known GPS data
                    self.last_lat = lat
                    self.last_lon = lon
                    self.last_alt = alt

            except Exception as e:
                self.get_logger().error(f"Error while receiving GPS data: {e}")

            time.sleep(0.1)  # Add a slight delay to avoid overloading CPU

    def publish_gps_data_thread(self):
        while True:
            # Publish data at 4Hz (0.25 seconds)
            if self.last_lat is not None and self.last_lon is not None and self.last_alt is not None:
                msg = NavSatFix()
                msg.latitude = self.last_lat / 1e7  # Convert to decimal degrees
                msg.longitude = self.last_lon / 1e7  # Convert to decimal degrees
                msg.altitude = self.last_alt / 1000  # Convert mm to meters
                msg.position_covariance_type = NavSatFix.COVARIANCE_TYPE_APPROXIMATED
                msg.header.frame_id = 'zed_imu_link'
                self.gps_pub.publish(msg)
            else:
                self.get_logger().warn('No valid GPS data to publish')

            time.sleep(0.034)  # 4Hz publishing rate\

def main(args=None):
    rclpy.init(args=args)

    # Create and start the UBX parser node
    ubx_node = UbxParserNode()

    # Spin the node to keep it alive
    rclpy.spin(ubx_node)

    # Clean up
    ubx_node.destroy_node()
    rclpy.shutdown()
    sys.exit(0)

if __name__ == '__main__':
    main()
