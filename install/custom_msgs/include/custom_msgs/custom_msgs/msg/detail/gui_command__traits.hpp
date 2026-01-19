// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from custom_msgs:msg/GuiCommand.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__TRAITS_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "custom_msgs/msg/detail/gui_command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace custom_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const GuiCommand & msg,
  std::ostream & out)
{
  out << "{";
  // member: nav_mode
  {
    out << "nav_mode: ";
    rosidl_generator_traits::value_to_yaml(msg.nav_mode, out);
    out << ", ";
  }

  // member: goal_lat
  {
    out << "goal_lat: ";
    rosidl_generator_traits::value_to_yaml(msg.goal_lat, out);
    out << ", ";
  }

  // member: goal_lon
  {
    out << "goal_lon: ";
    rosidl_generator_traits::value_to_yaml(msg.goal_lon, out);
    out << ", ";
  }

  // member: target_cone_id
  {
    out << "target_cone_id: ";
    rosidl_generator_traits::value_to_yaml(msg.target_cone_id, out);
    out << ", ";
  }

  // member: set_search_skew
  {
    out << "set_search_skew: ";
    rosidl_generator_traits::value_to_yaml(msg.set_search_skew, out);
    out << ", ";
  }

  // member: search_skew
  {
    out << "search_skew: ";
    rosidl_generator_traits::value_to_yaml(msg.search_skew, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GuiCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: nav_mode
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "nav_mode: ";
    rosidl_generator_traits::value_to_yaml(msg.nav_mode, out);
    out << "\n";
  }

  // member: goal_lat
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal_lat: ";
    rosidl_generator_traits::value_to_yaml(msg.goal_lat, out);
    out << "\n";
  }

  // member: goal_lon
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal_lon: ";
    rosidl_generator_traits::value_to_yaml(msg.goal_lon, out);
    out << "\n";
  }

  // member: target_cone_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_cone_id: ";
    rosidl_generator_traits::value_to_yaml(msg.target_cone_id, out);
    out << "\n";
  }

  // member: set_search_skew
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "set_search_skew: ";
    rosidl_generator_traits::value_to_yaml(msg.set_search_skew, out);
    out << "\n";
  }

  // member: search_skew
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "search_skew: ";
    rosidl_generator_traits::value_to_yaml(msg.search_skew, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GuiCommand & msg, bool use_flow_style = false)
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
  const custom_msgs::msg::GuiCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  custom_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use custom_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const custom_msgs::msg::GuiCommand & msg)
{
  return custom_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<custom_msgs::msg::GuiCommand>()
{
  return "custom_msgs::msg::GuiCommand";
}

template<>
inline const char * name<custom_msgs::msg::GuiCommand>()
{
  return "custom_msgs/msg/GuiCommand";
}

template<>
struct has_fixed_size<custom_msgs::msg::GuiCommand>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<custom_msgs::msg::GuiCommand>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<custom_msgs::msg::GuiCommand>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__TRAITS_HPP_
