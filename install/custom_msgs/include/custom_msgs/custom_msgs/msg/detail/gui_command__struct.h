// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from custom_msgs:msg/GuiCommand.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__STRUCT_H_
#define CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/GuiCommand in the package custom_msgs.
typedef struct custom_msgs__msg__GuiCommand
{
  int8_t nav_mode;
  double goal_lat;
  double goal_lon;
  int32_t target_cone_id;
  bool set_search_skew;
  int8_t search_skew;
} custom_msgs__msg__GuiCommand;

// Struct for a sequence of custom_msgs__msg__GuiCommand.
typedef struct custom_msgs__msg__GuiCommand__Sequence
{
  custom_msgs__msg__GuiCommand * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} custom_msgs__msg__GuiCommand__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__STRUCT_H_
