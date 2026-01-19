// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from custom_msgs:msg/PlannerStatus.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__STRUCT_H_
#define CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/PlannerStatus in the package custom_msgs.
typedef struct custom_msgs__msg__PlannerStatus
{
  int8_t state;
  int8_t nav_mode;
  double curr_lat;
  double curr_lon;
  double goal_lat;
  double goal_lon;
  double distance_to_goal_m;
  double current_yaw_deg;
  double target_yaw_deg;
  double heading_error_deg;
  bool cone_detected;
  double cone_x;
  double cone_y;
  int32_t target_cone_id;
  bool obstacle_detected;
  bool gps_goal_reached;
  bool cone_goal_reached;
  bool autonomous_enabled;
  builtin_interfaces__msg__Time stamp;
} custom_msgs__msg__PlannerStatus;

// Struct for a sequence of custom_msgs__msg__PlannerStatus.
typedef struct custom_msgs__msg__PlannerStatus__Sequence
{
  custom_msgs__msg__PlannerStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} custom_msgs__msg__PlannerStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CUSTOM_MSGS__MSG__DETAIL__PLANNER_STATUS__STRUCT_H_
