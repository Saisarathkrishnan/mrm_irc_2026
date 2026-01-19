// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from custom_msgs:msg/ArmPwm.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__TRAITS_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "custom_msgs/msg/detail/arm_pwm__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace custom_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ArmPwm & msg,
  std::ostream & out)
{
  out << "{";
  // member: link1
  {
    out << "link1: ";
    rosidl_generator_traits::value_to_yaml(msg.link1, out);
    out << ", ";
  }

  // member: link2
  {
    out << "link2: ";
    rosidl_generator_traits::value_to_yaml(msg.link2, out);
    out << ", ";
  }

  // member: swivel
  {
    out << "swivel: ";
    rosidl_generator_traits::value_to_yaml(msg.swivel, out);
    out << ", ";
  }

  // member: gripper
  {
    out << "gripper: ";
    rosidl_generator_traits::value_to_yaml(msg.gripper, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ArmPwm & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: link1
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "link1: ";
    rosidl_generator_traits::value_to_yaml(msg.link1, out);
    out << "\n";
  }

  // member: link2
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "link2: ";
    rosidl_generator_traits::value_to_yaml(msg.link2, out);
    out << "\n";
  }

  // member: swivel
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "swivel: ";
    rosidl_generator_traits::value_to_yaml(msg.swivel, out);
    out << "\n";
  }

  // member: gripper
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "gripper: ";
    rosidl_generator_traits::value_to_yaml(msg.gripper, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ArmPwm & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace custom_msgs

namespace rosidl_generator_traits
{

[[deprecated("use custom_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const custom_msgs::msg::ArmPwm & msg,
  std::ostream & out, size_t indentation = 0)
{
  custom_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use custom_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const custom_msgs::msg::ArmPwm & msg)
{
  return custom_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<custom_msgs::msg::ArmPwm>()
{
  return "custom_msgs::msg::ArmPwm";
}

template<>
inline const char * name<custom_msgs::msg::ArmPwm>()
{
  return "custom_msgs/msg/ArmPwm";
}

template<>
struct has_fixed_size<custom_msgs::msg::ArmPwm>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<custom_msgs::msg::ArmPwm>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<custom_msgs::msg::ArmPwm>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__TRAITS_HPP_
