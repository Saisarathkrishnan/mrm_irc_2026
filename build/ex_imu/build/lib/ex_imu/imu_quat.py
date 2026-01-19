import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu
import board
import adafruit_bno055
import time
import pickle
import os
import math

def euler_to_quaternion(roll, pitch, yaw):
    """
    Convert Euler angles (degrees) to quaternion
    Roll, Pitch, Yaw are in degrees
    Returns x, y, z, w
    """
    # Convert degrees to radians
    roll = math.radians(roll)
    pitch = math.radians(pitch)
    yaw = math.radians(yaw)

    cy = math.cos(yaw * 0.5)
    sy = math.sin(yaw * 0.5)
    cp = math.cos(pitch * 0.5)
    sp = math.sin(pitch * 0.5)
    cr = math.cos(roll * 0.5)
    sr = math.sin(roll * 0.5)

    w = cr * cp * cy + sr * sp * sy
    x = sr * cp * cy - cr * sp * sy
    y = cr * sp * cy + sr * cp * sy
    z = cr * cp * sy - sr * sp * cy

    return x, y, z, w


class BNO055ImuNode(Node):
    def __init__(self):
        super().__init__('bno055_imu_node')

        # -------------------------
        # ROS Publisher
        # -------------------------
        self.imu_pub_ = self.create_publisher(Imu, '/imu/data', 10)

        # -------------------------
        # CONFIG
        # -------------------------
        self.calib_file = os.path.expanduser("~/imu_calib.pkl")
        self.publish_hz = 50.0  # IMU publish rate
        self.use_west_zero = True

        # -------------------------
        # INIT SENSOR
        # -------------------------
        self.i2c = board.I2C()
        self.sensor = adafruit_bno055.BNO055_I2C(self.i2c)
        self.sensor.mode = adafruit_bno055.NDOF_MODE
        time.sleep(1)

        # -------------------------
        # LOAD CALIBRATION
        # -------------------------
        if os.path.exists(self.calib_file):
            try:
                with open(self.calib_file, "rb") as f:
                    self.sensor.calibration = pickle.load(f)
                self.get_logger().info("Calibration loaded")
            except Exception as e:
                self.get_logger().warn(f"Failed to load calibration: {e}")
        else:
            self.get_logger().warn("No calibration file found")

        # -------------------------
        # WAIT FOR USABLE YAW
        # -------------------------
        self.get_logger().info("Waiting for usable yaw...")
        while True:
            sys, gyro, accel, mag = self.sensor.calibration_status
            if gyro == 3 and mag == 3:
                break
            time.sleep(0.2)
        self.get_logger().info("Yaw usable")

        # -------------------------
        # Start timer
        # -------------------------
        self.timer_ = self.create_timer(1.0 / self.publish_hz, self.publish_imu)

    def publish_imu(self):
        euler = self.sensor.euler
        lin_accel = self.sensor.linear_acceleration
        ang_vel = self.sensor.gyro  # optional angular velocity in deg/s

        # Skip if data not ready
        if euler is None or lin_accel is None or any(x is None for x in euler + lin_accel):
            return

        yaw, pitch, roll = euler
        ax, ay, az = lin_accel

        # Convert to quaternion
        qx, qy, qz, qw = euler_to_quaternion(roll, pitch, yaw)

        # Create Imu message
        imu_msg = Imu()
        imu_msg.header.stamp = self.get_clock().now().to_msg()
        imu_msg.header.frame_id = "imu_link"

        # Orientation
        imu_msg.orientation.x = qx
        imu_msg.orientation.y = qy
        imu_msg.orientation.z = qz
        imu_msg.orientation.w = qw

        # Linear acceleration (m/s^2)
        imu_msg.linear_acceleration.x = ax
        imu_msg.linear_acceleration.y = ay
        imu_msg.linear_acceleration.z = az

        # Angular velocity (rad/s)
        if ang_vel is not None and all(v is not None for v in ang_vel):
            imu_msg.angular_velocity.x = math.radians(ang_vel[0])
            imu_msg.angular_velocity.y = math.radians(ang_vel[1])
            imu_msg.angular_velocity.z = math.radians(ang_vel[2])
        else:
            imu_msg.angular_velocity.x = 0.0
            imu_msg.angular_velocity.y = 0.0
            imu_msg.angular_velocity.z = 0.0

        # Publish
        self.imu_pub_.publish(imu_msg)

    def save_calibration(self):
        try:
            with open(self.calib_file, "wb") as f:
                pickle.dump(self.sensor.calibration, f)
            self.get_logger().info("Calibration saved")
        except Exception as e:
            self.get_logger().warn(f"Failed to save calibration: {e}")


def main(args=None):
    rclpy.init(args=args)
    node = BNO055ImuNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.save_calibration()
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
