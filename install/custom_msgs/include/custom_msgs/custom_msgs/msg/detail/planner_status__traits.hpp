// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from custom_msgs:msg/PlannerStatus.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__TRAITS_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "custom_msgs/msg/detail/planner_status__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace custom_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const PlannerStatus & msg,
  std::ostream & out)
{
  out << "{";
  // member: state
  {
    out << "state: ";
    rosidl_generator_traits::value_to_yaml(msg.state, out);
    out << ", ";
  }

  // member: nav_mode
  {
    out << "nav_mode: ";
    rosidl_generator_traits::value_to_yaml(msg.nav_mode, out);
    out << ", ";
  }

  // member: curr_lat
  {
    out << "curr_lat: ";
    rosidl_generator_traits::value_to_yaml(msg.curr_lat, out);
    out << ", ";
  }

  // member: curr_lon
  {
    out << "curr_lon: ";
    rosidl_generator_traits::value_to_yaml(msg.curr_lon, out);
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

  // member: distance_to_goal_m
  {
    out << "distance_to_goal_m: ";
    rosidl_generator_traits::value_to_yaml(msg.distance_to_goal_m, out);
    out << ", ";
  }

  // member: current_yaw_deg
  {
    out << "current_yaw_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.current_yaw_deg, out);
    out << ", ";
  }

  // member: target_yaw_deg
  {
    out << "target_yaw_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.target_yaw_deg, out);
    out << ", ";
  }

  // member: heading_error_deg
  {
    out << "heading_error_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.heading_error_deg, out);
    out << ", ";
  }

  // member: cone_detected
  {
    out << "cone_detected: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_detected, out);
    out << ", ";
  }

  // member: cone_x
  {
    out << "cone_x: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_x, out);
    out << ", ";
  }

  // member: cone_y
  {
    out << "cone_y: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_y, out);
    out << ", ";
  }

  // member: target_cone_id
  {
    out << "target_cone_id: ";
    rosidl_generator_traits::value_to_yaml(msg.target_cone_id, out);
    out << ", ";
  }

  // member: obstacle_detected
  {
    out << "obstacle_detected: ";
    rosidl_generator_traits::value_to_yaml(msg.obstacle_detected, out);
    out << ", ";
  }

  // member: gps_goal_reached
  {
    out << "gps_goal_reached: ";
    rosidl_generator_traits::value_to_yaml(msg.gps_goal_reached, out);
    out << ", ";
  }

  // member: cone_goal_reached
  {
    out << "cone_goal_reached: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_goal_reached, out);
    out << ", ";
  }

  // member: autonomous_enabled
  {
    out << "autonomous_enabled: ";
    rosidl_generator_traits::value_to_yaml(msg.autonomous_enabled, out);
    out << ", ";
  }

  // member: stamp
  {
    out << "stamp: ";
    to_flow_style_yaml(msg.stamp, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PlannerStatus & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: state
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "state: ";
    rosidl_generator_traits::value_to_yaml(msg.state, out);
    out << "\n";
  }

  // member: nav_mode
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "nav_mode: ";
    rosidl_generator_traits::value_to_yaml(msg.nav_mode, out);
    out << "\n";
  }

  // member: curr_lat
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "curr_lat: ";
    rosidl_generator_traits::value_to_yaml(msg.curr_lat, out);
    out << "\n";
  }

  // member: curr_lon
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "curr_lon: ";
    rosidl_generator_traits::value_to_yaml(msg.curr_lon, out);
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

  // member: distance_to_goal_m
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "distance_to_goal_m: ";
    rosidl_generator_traits::value_to_yaml(msg.distance_to_goal_m, out);
    out << "\n";
  }

  // member: current_yaw_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "current_yaw_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.current_yaw_deg, out);
    out << "\n";
  }

  // member: target_yaw_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_yaw_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.target_yaw_deg, out);
    out << "\n";
  }

  // member: heading_error_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "heading_error_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.heading_error_deg, out);
    out << "\n";
  }

  // member: cone_detected
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cone_detected: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_detected, out);
    out << "\n";
  }

  // member: cone_x
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cone_x: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_x, out);
    out << "\n";
  }

  // member: cone_y
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cone_y: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_y, out);
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

  // member: obstacle_detected
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "obstacle_detected: ";
    rosidl_generator_traits::value_to_yaml(msg.obstacle_detected, out);
    out << "\n";
  }

  // member: gps_goal_reached
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "gps_goal_reached: ";
    rosidl_generator_traits::value_to_yaml(msg.gps_goal_reached, out);
    out << "\n";
  }

  // member: cone_goal_reached
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cone_goal_reached: ";
    rosidl_generator_traits::value_to_yaml(msg.cone_goal_reached, out);
    out << "\n";
  }

  // member: autonomous_enabled
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "autonomous_enabled: ";
    rosidl_generator_traits::value_to_yaml(msg.autonomous_enabled, out);
    out << "\n";
  }

  // member: stamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "stamp:\n";
    to_block_style_yaml(msg.stamp, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PlannerStatus & msg, bool use_flow_style = false)
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
  const custom_msgs::msg::PlannerStatus & msg,
  std::ostream & out, size_t indentation = 0)
{
  custom_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use custom_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const custom_msgs::msg::PlannerStatus & msg)
{
  return custom_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<custom_msgs::msg::PlannerStatus>()
{
  return "custom_msgs::msg::PlannerStatus";
}

template<>
inline const char * name<custom_msgs::msg::PlannerStatus>()
{
  return "custom_msgs/msg/PlannerStatus";
}

template<>
struct has_fixed_size<custom_msgs::msg::PlannerStatus>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct has_bounded_size<custom_msgs::msg::PlannerStatus>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct is_message<custom_msgs::msg::PlannerStatus>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__TRAITS_HPP_
