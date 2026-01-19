// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from custom_msgs:msg/GpsDetails.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__TRAITS_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "custom_msgs/msg/detail/gps_details__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace custom_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const GpsDetails & msg,
  std::ostream & out)
{
  out << "{";
  // member: latitude
  {
    out << "latitude: ";
    rosidl_generator_traits::value_to_yaml(msg.latitude, out);
    out << ", ";
  }

  // member: longitude
  {
    out << "longitude: ";
    rosidl_generator_traits::value_to_yaml(msg.longitude, out);
    out << ", ";
  }

  // member: altitude
  {
    out << "altitude: ";
    rosidl_generator_traits::value_to_yaml(msg.altitude, out);
    out << ", ";
  }

  // member: fix_type
  {
    out << "fix_type: ";
    rosidl_generator_traits::value_to_yaml(msg.fix_type, out);
    out << ", ";
  }

  // member: satellites
  {
    out << "satellites: ";
    rosidl_generator_traits::value_to_yaml(msg.satellites, out);
    out << ", ";
  }

  // member: horizontal_accuracy
  {
    out << "horizontal_accuracy: ";
    rosidl_generator_traits::value_to_yaml(msg.horizontal_accuracy, out);
    out << ", ";
  }

  // member: vertical_accuracy
  {
    out << "vertical_accuracy: ";
    rosidl_generator_traits::value_to_yaml(msg.vertical_accuracy, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GpsDetails & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: latitude
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "latitude: ";
    rosidl_generator_traits::value_to_yaml(msg.latitude, out);
    out << "\n";
  }

  // member: longitude
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "longitude: ";
    rosidl_generator_traits::value_to_yaml(msg.longitude, out);
    out << "\n";
  }

  // member: altitude
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "altitude: ";
    rosidl_generator_traits::value_to_yaml(msg.altitude, out);
    out << "\n";
  }

  // member: fix_type
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "fix_type: ";
    rosidl_generator_traits::value_to_yaml(msg.fix_type, out);
    out << "\n";
  }

  // member: satellites
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "satellites: ";
    rosidl_generator_traits::value_to_yaml(msg.satellites, out);
    out << "\n";
  }

  // member: horizontal_accuracy
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "horizontal_accuracy: ";
    rosidl_generator_traits::value_to_yaml(msg.horizontal_accuracy, out);
    out << "\n";
  }

  // member: vertical_accuracy
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "vertical_accuracy: ";
    rosidl_generator_traits::value_to_yaml(msg.vertical_accuracy, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GpsDetails & msg, bool use_flow_style = false)
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
  const custom_msgs::msg::GpsDetails & msg,
  std::ostream & out, size_t indentation = 0)
{
  custom_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use custom_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const custom_msgs::msg::GpsDetails & msg)
{
  return custom_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<custom_msgs::msg::GpsDetails>()
{
  return "custom_msgs::msg::GpsDetails";
}

template<>
inline const char * name<custom_msgs::msg::GpsDetails>()
{
  return "custom_msgs/msg/GpsDetails";
}

template<>
struct has_fixed_size<custom_msgs::msg::GpsDetails>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<custom_msgs::msg::GpsDetails>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<custom_msgs::msg::GpsDetails>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__TRAITS_HPP_
