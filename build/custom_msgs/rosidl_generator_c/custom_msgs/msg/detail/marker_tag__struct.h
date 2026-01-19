// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from custom_msgs:msg/MarkerTag.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__MARKER_TAG__STRUCT_H_
#define CUSTOM_MSGS__MSG__DETAIL__MARKER_TAG__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/MarkerTag in the package custom_msgs.
typedef struct custom_msgs__msg__MarkerTag
{
  bool is_found;
  int64_t id;
  double x;
  double y;
} custom_msgs__msg__MarkerTag;

// Struct for a sequence of custom_msgs__msg__MarkerTag.
typedef struct custom_msgs__msg__MarkerTag__Sequence
{
  custom_msgs__msg__MarkerTag * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} custom_msgs__msg__MarkerTag__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CUSTOM_MSGS__MSG__DETAIL__MARKER_TAG__STRUCT_H_
