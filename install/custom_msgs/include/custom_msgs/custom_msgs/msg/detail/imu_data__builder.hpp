// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/ImuData.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__IMU_DATA__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__IMU_DATA__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/imu_data__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_ImuData_orientation
{
public:
  explicit Init_ImuData_orientation(::custom_msgs::msg::ImuData & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::ImuData orientation(::custom_msgs::msg::ImuData::_orientation_type arg)
  {
    msg_.orientation = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::ImuData msg_;
};

class Init_ImuData_acceleration
{
public:
  Init_ImuData_acceleration()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ImuData_orientation acceleration(::custom_msgs::msg::ImuData::_acceleration_type arg)
  {
    msg_.acceleration = std::move(arg);
    return Init_ImuData_orientation(msg_);
  }

private:
  ::custom_msgs::msg::ImuData msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::ImuData>()
{
  return custom_msgs::msg::builder::Init_ImuData_acceleration();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__IMU_DATA__BUILDER_HPP_
