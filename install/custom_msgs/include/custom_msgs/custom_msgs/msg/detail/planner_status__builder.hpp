// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/PlannerStatus.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/planner_status__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_PlannerStatus_stamp
{
public:
  explicit Init_PlannerStatus_stamp(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::PlannerStatus stamp(::custom_msgs::msg::PlannerStatus::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_autonomous_enabled
{
public:
  explicit Init_PlannerStatus_autonomous_enabled(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_stamp autonomous_enabled(::custom_msgs::msg::PlannerStatus::_autonomous_enabled_type arg)
  {
    msg_.autonomous_enabled = std::move(arg);
    return Init_PlannerStatus_stamp(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_cone_goal_reached
{
public:
  explicit Init_PlannerStatus_cone_goal_reached(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_autonomous_enabled cone_goal_reached(::custom_msgs::msg::PlannerStatus::_cone_goal_reached_type arg)
  {
    msg_.cone_goal_reached = std::move(arg);
    return Init_PlannerStatus_autonomous_enabled(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_gps_goal_reached
{
public:
  explicit Init_PlannerStatus_gps_goal_reached(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_cone_goal_reached gps_goal_reached(::custom_msgs::msg::PlannerStatus::_gps_goal_reached_type arg)
  {
    msg_.gps_goal_reached = std::move(arg);
    return Init_PlannerStatus_cone_goal_reached(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_obstacle_detected
{
public:
  explicit Init_PlannerStatus_obstacle_detected(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_gps_goal_reached obstacle_detected(::custom_msgs::msg::PlannerStatus::_obstacle_detected_type arg)
  {
    msg_.obstacle_detected = std::move(arg);
    return Init_PlannerStatus_gps_goal_reached(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_target_cone_id
{
public:
  explicit Init_PlannerStatus_target_cone_id(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_obstacle_detected target_cone_id(::custom_msgs::msg::PlannerStatus::_target_cone_id_type arg)
  {
    msg_.target_cone_id = std::move(arg);
    return Init_PlannerStatus_obstacle_detected(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_cone_y
{
public:
  explicit Init_PlannerStatus_cone_y(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_target_cone_id cone_y(::custom_msgs::msg::PlannerStatus::_cone_y_type arg)
  {
    msg_.cone_y = std::move(arg);
    return Init_PlannerStatus_target_cone_id(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_cone_x
{
public:
  explicit Init_PlannerStatus_cone_x(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_cone_y cone_x(::custom_msgs::msg::PlannerStatus::_cone_x_type arg)
  {
    msg_.cone_x = std::move(arg);
    return Init_PlannerStatus_cone_y(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_cone_detected
{
public:
  explicit Init_PlannerStatus_cone_detected(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_cone_x cone_detected(::custom_msgs::msg::PlannerStatus::_cone_detected_type arg)
  {
    msg_.cone_detected = std::move(arg);
    return Init_PlannerStatus_cone_x(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_heading_error_deg
{
public:
  explicit Init_PlannerStatus_heading_error_deg(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_cone_detected heading_error_deg(::custom_msgs::msg::PlannerStatus::_heading_error_deg_type arg)
  {
    msg_.heading_error_deg = std::move(arg);
    return Init_PlannerStatus_cone_detected(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_target_yaw_deg
{
public:
  explicit Init_PlannerStatus_target_yaw_deg(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_heading_error_deg target_yaw_deg(::custom_msgs::msg::PlannerStatus::_target_yaw_deg_type arg)
  {
    msg_.target_yaw_deg = std::move(arg);
    return Init_PlannerStatus_heading_error_deg(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_current_yaw_deg
{
public:
  explicit Init_PlannerStatus_current_yaw_deg(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_target_yaw_deg current_yaw_deg(::custom_msgs::msg::PlannerStatus::_current_yaw_deg_type arg)
  {
    msg_.current_yaw_deg = std::move(arg);
    return Init_PlannerStatus_target_yaw_deg(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_distance_to_goal_m
{
public:
  explicit Init_PlannerStatus_distance_to_goal_m(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_current_yaw_deg distance_to_goal_m(::custom_msgs::msg::PlannerStatus::_distance_to_goal_m_type arg)
  {
    msg_.distance_to_goal_m = std::move(arg);
    return Init_PlannerStatus_current_yaw_deg(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_goal_lon
{
public:
  explicit Init_PlannerStatus_goal_lon(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_distance_to_goal_m goal_lon(::custom_msgs::msg::PlannerStatus::_goal_lon_type arg)
  {
    msg_.goal_lon = std::move(arg);
    return Init_PlannerStatus_distance_to_goal_m(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_goal_lat
{
public:
  explicit Init_PlannerStatus_goal_lat(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_goal_lon goal_lat(::custom_msgs::msg::PlannerStatus::_goal_lat_type arg)
  {
    msg_.goal_lat = std::move(arg);
    return Init_PlannerStatus_goal_lon(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_curr_lon
{
public:
  explicit Init_PlannerStatus_curr_lon(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_goal_lat curr_lon(::custom_msgs::msg::PlannerStatus::_curr_lon_type arg)
  {
    msg_.curr_lon = std::move(arg);
    return Init_PlannerStatus_goal_lat(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_curr_lat
{
public:
  explicit Init_PlannerStatus_curr_lat(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_curr_lon curr_lat(::custom_msgs::msg::PlannerStatus::_curr_lat_type arg)
  {
    msg_.curr_lat = std::move(arg);
    return Init_PlannerStatus_curr_lon(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_nav_mode
{
public:
  explicit Init_PlannerStatus_nav_mode(::custom_msgs::msg::PlannerStatus & msg)
  : msg_(msg)
  {}
  Init_PlannerStatus_curr_lat nav_mode(::custom_msgs::msg::PlannerStatus::_nav_mode_type arg)
  {
    msg_.nav_mode = std::move(arg);
    return Init_PlannerStatus_curr_lat(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

class Init_PlannerStatus_state
{
public:
  Init_PlannerStatus_state()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PlannerStatus_nav_mode state(::custom_msgs::msg::PlannerStatus::_state_type arg)
  {
    msg_.state = std::move(arg);
    return Init_PlannerStatus_nav_mode(msg_);
  }

private:
  ::custom_msgs::msg::PlannerStatus msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::PlannerStatus>()
{
  return custom_msgs::msg::builder::Init_PlannerStatus_state();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__BUILDER_HPP_
