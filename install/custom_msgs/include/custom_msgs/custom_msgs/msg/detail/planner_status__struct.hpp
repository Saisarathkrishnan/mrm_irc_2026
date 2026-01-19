// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from custom_msgs:msg/PlannerStatus.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__STRUCT_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__custom_msgs__msg__PlannerStatus __attribute__((deprecated))
#else
# define DEPRECATED__custom_msgs__msg__PlannerStatus __declspec(deprecated)
#endif

namespace custom_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct PlannerStatus_
{
  using Type = PlannerStatus_<ContainerAllocator>;

  explicit PlannerStatus_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->state = 0;
      this->nav_mode = 0;
      this->curr_lat = 0.0;
      this->curr_lon = 0.0;
      this->goal_lat = 0.0;
      this->goal_lon = 0.0;
      this->distance_to_goal_m = 0.0;
      this->current_yaw_deg = 0.0;
      this->target_yaw_deg = 0.0;
      this->heading_error_deg = 0.0;
      this->cone_detected = false;
      this->cone_x = 0.0;
      this->cone_y = 0.0;
      this->target_cone_id = 0l;
      this->obstacle_detected = false;
      this->gps_goal_reached = false;
      this->cone_goal_reached = false;
      this->autonomous_enabled = false;
    }
  }

  explicit PlannerStatus_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->state = 0;
      this->nav_mode = 0;
      this->curr_lat = 0.0;
      this->curr_lon = 0.0;
      this->goal_lat = 0.0;
      this->goal_lon = 0.0;
      this->distance_to_goal_m = 0.0;
      this->current_yaw_deg = 0.0;
      this->target_yaw_deg = 0.0;
      this->heading_error_deg = 0.0;
      this->cone_detected = false;
      this->cone_x = 0.0;
      this->cone_y = 0.0;
      this->target_cone_id = 0l;
      this->obstacle_detected = false;
      this->gps_goal_reached = false;
      this->cone_goal_reached = false;
      this->autonomous_enabled = false;
    }
  }

  // field types and members
  using _state_type =
    int8_t;
  _state_type state;
  using _nav_mode_type =
    int8_t;
  _nav_mode_type nav_mode;
  using _curr_lat_type =
    double;
  _curr_lat_type curr_lat;
  using _curr_lon_type =
    double;
  _curr_lon_type curr_lon;
  using _goal_lat_type =
    double;
  _goal_lat_type goal_lat;
  using _goal_lon_type =
    double;
  _goal_lon_type goal_lon;
  using _distance_to_goal_m_type =
    double;
  _distance_to_goal_m_type distance_to_goal_m;
  using _current_yaw_deg_type =
    double;
  _current_yaw_deg_type current_yaw_deg;
  using _target_yaw_deg_type =
    double;
  _target_yaw_deg_type target_yaw_deg;
  using _heading_error_deg_type =
    double;
  _heading_error_deg_type heading_error_deg;
  using _cone_detected_type =
    bool;
  _cone_detected_type cone_detected;
  using _cone_x_type =
    double;
  _cone_x_type cone_x;
  using _cone_y_type =
    double;
  _cone_y_type cone_y;
  using _target_cone_id_type =
    int32_t;
  _target_cone_id_type target_cone_id;
  using _obstacle_detected_type =
    bool;
  _obstacle_detected_type obstacle_detected;
  using _gps_goal_reached_type =
    bool;
  _gps_goal_reached_type gps_goal_reached;
  using _cone_goal_reached_type =
    bool;
  _cone_goal_reached_type cone_goal_reached;
  using _autonomous_enabled_type =
    bool;
  _autonomous_enabled_type autonomous_enabled;
  using _stamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _stamp_type stamp;

  // setters for named parameter idiom
  Type & set__state(
    const int8_t & _arg)
  {
    this->state = _arg;
    return *this;
  }
  Type & set__nav_mode(
    const int8_t & _arg)
  {
    this->nav_mode = _arg;
    return *this;
  }
  Type & set__curr_lat(
    const double & _arg)
  {
    this->curr_lat = _arg;
    return *this;
  }
  Type & set__curr_lon(
    const double & _arg)
  {
    this->curr_lon = _arg;
    return *this;
  }
  Type & set__goal_lat(
    const double & _arg)
  {
    this->goal_lat = _arg;
    return *this;
  }
  Type & set__goal_lon(
    const double & _arg)
  {
    this->goal_lon = _arg;
    return *this;
  }
  Type & set__distance_to_goal_m(
    const double & _arg)
  {
    this->distance_to_goal_m = _arg;
    return *this;
  }
  Type & set__current_yaw_deg(
    const double & _arg)
  {
    this->current_yaw_deg = _arg;
    return *this;
  }
  Type & set__target_yaw_deg(
    const double & _arg)
  {
    this->target_yaw_deg = _arg;
    return *this;
  }
  Type & set__heading_error_deg(
    const double & _arg)
  {
    this->heading_error_deg = _arg;
    return *this;
  }
  Type & set__cone_detected(
    const bool & _arg)
  {
    this->cone_detected = _arg;
    return *this;
  }
  Type & set__cone_x(
    const double & _arg)
  {
    this->cone_x = _arg;
    return *this;
  }
  Type & set__cone_y(
    const double & _arg)
  {
    this->cone_y = _arg;
    return *this;
  }
  Type & set__target_cone_id(
    const int32_t & _arg)
  {
    this->target_cone_id = _arg;
    return *this;
  }
  Type & set__obstacle_detected(
    const bool & _arg)
  {
    this->obstacle_detected = _arg;
    return *this;
  }
  Type & set__gps_goal_reached(
    const bool & _arg)
  {
    this->gps_goal_reached = _arg;
    return *this;
  }
  Type & set__cone_goal_reached(
    const bool & _arg)
  {
    this->cone_goal_reached = _arg;
    return *this;
  }
  Type & set__autonomous_enabled(
    const bool & _arg)
  {
    this->autonomous_enabled = _arg;
    return *this;
  }
  Type & set__stamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->stamp = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    custom_msgs::msg::PlannerStatus_<ContainerAllocator> *;
  using ConstRawPtr =
    const custom_msgs::msg::PlannerStatus_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::PlannerStatus_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::PlannerStatus_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__custom_msgs__msg__PlannerStatus
    std::shared_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__custom_msgs__msg__PlannerStatus
    std::shared_ptr<custom_msgs::msg::PlannerStatus_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const PlannerStatus_ & other) const
  {
    if (this->state != other.state) {
      return false;
    }
    if (this->nav_mode != other.nav_mode) {
      return false;
    }
    if (this->curr_lat != other.curr_lat) {
      return false;
    }
    if (this->curr_lon != other.curr_lon) {
      return false;
    }
    if (this->goal_lat != other.goal_lat) {
      return false;
    }
    if (this->goal_lon != other.goal_lon) {
      return false;
    }
    if (this->distance_to_goal_m != other.distance_to_goal_m) {
      return false;
    }
    if (this->current_yaw_deg != other.current_yaw_deg) {
      return false;
    }
    if (this->target_yaw_deg != other.target_yaw_deg) {
      return false;
    }
    if (this->heading_error_deg != other.heading_error_deg) {
      return false;
    }
    if (this->cone_detected != other.cone_detected) {
      return false;
    }
    if (this->cone_x != other.cone_x) {
      return false;
    }
    if (this->cone_y != other.cone_y) {
      return false;
    }
    if (this->target_cone_id != other.target_cone_id) {
      return false;
    }
    if (this->obstacle_detected != other.obstacle_detected) {
      return false;
    }
    if (this->gps_goal_reached != other.gps_goal_reached) {
      return false;
    }
    if (this->cone_goal_reached != other.cone_goal_reached) {
      return false;
    }
    if (this->autonomous_enabled != other.autonomous_enabled) {
      return false;
    }
    if (this->stamp != other.stamp) {
      return false;
    }
    return true;
  }
  bool operator!=(const PlannerStatus_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct PlannerStatus_

// alias to use template instance with default allocator
using PlannerStatus =
  custom_msgs::msg::PlannerStatus_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__STRUCT_HPP_
