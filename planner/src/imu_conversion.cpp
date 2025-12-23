#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include "custom_msgs/msg/imu_data.hpp"

class ImuConversionNode : public rclcpp::Node
{
public:
  ImuConversionNode() : Node("imu_convert")
  {
    imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
        "/zed/zed_node/imu/data", 10,
        std::bind(&ImuConversionNode::imuCallback, this, std::placeholders::_1));

    imu_pub_ = this->create_publisher<custom_msgs::msg::ImuData>("/imu_data", 10);

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(16),
        std::bind(&ImuConversionNode::publishImuData, this));
  }

private:
  void imuCallback(const sensor_msgs::msg::Imu::SharedPtr msg)
  {
    tf2::Quaternion q(
        msg->orientation.x,
        msg->orientation.y,
        msg->orientation.z,
        msg->orientation.w);

    tf2::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    // Normalize (0–360 degrees)
    if (roll < 0) roll += M_PI * 2;
    if (pitch < 0) pitch += M_PI * 2;
    if (yaw < 0) yaw += M_PI * 2;

    // Convert to degrees
    roll *= 180.0 / M_PI;
    pitch *= 180.0 / M_PI;
    yaw *= 180.0 / M_PI;

    // Fill message
    imu_msg_.orientation.x = roll;
    imu_msg_.orientation.y = pitch;
    imu_msg_.orientation.z = yaw;

    imu_msg_.acceleration.x = msg->linear_acceleration.x;
    imu_msg_.acceleration.y = msg->linear_acceleration.y;
    imu_msg_.acceleration.z = msg->linear_acceleration.z;
  }

  void publishImuData()
  {
    imu_pub_->publish(imu_msg_);
  }

  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
  rclcpp::Publisher<custom_msgs::msg::ImuData>::SharedPtr imu_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  custom_msgs::msg::ImuData imu_msg_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ImuConversionNode>());
  rclcpp::shutdown();
  return 0;
}
