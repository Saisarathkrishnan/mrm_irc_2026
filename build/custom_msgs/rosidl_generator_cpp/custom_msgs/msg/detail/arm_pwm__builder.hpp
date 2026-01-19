// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/ArmPwm.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/arm_pwm__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_ArmPwm_gripper
{
public:
  explicit Init_ArmPwm_gripper(::custom_msgs::msg::ArmPwm & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::ArmPwm gripper(::custom_msgs::msg::ArmPwm::_gripper_type arg)
  {
    msg_.gripper = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::ArmPwm msg_;
};

class Init_ArmPwm_swivel
{
public:
  explicit Init_ArmPwm_swivel(::custom_msgs::msg::ArmPwm & msg)
  : msg_(msg)
  {}
  Init_ArmPwm_gripper swivel(::custom_msgs::msg::ArmPwm::_swivel_type arg)
  {
    msg_.swivel = std::move(arg);
    return Init_ArmPwm_gripper(msg_);
  }

private:
  ::custom_msgs::msg::ArmPwm msg_;
};

class Init_ArmPwm_link2
{
public:
  explicit Init_ArmPwm_link2(::custom_msgs::msg::ArmPwm & msg)
  : msg_(msg)
  {}
  Init_ArmPwm_swivel link2(::custom_msgs::msg::ArmPwm::_link2_type arg)
  {
    msg_.link2 = std::move(arg);
    return Init_ArmPwm_swivel(msg_);
  }

private:
  ::custom_msgs::msg::ArmPwm msg_;
};

class Init_ArmPwm_link1
{
public:
  Init_ArmPwm_link1()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ArmPwm_link2 link1(::custom_msgs::msg::ArmPwm::_link1_type arg)
  {
    msg_.link1 = std::move(arg);
    return Init_ArmPwm_link2(msg_);
  }

private:
  ::custom_msgs::msg::ArmPwm msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::ArmPwm>()
{
  return custom_msgs::msg::builder::Init_ArmPwm_link1();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__BUILDER_HPP_
