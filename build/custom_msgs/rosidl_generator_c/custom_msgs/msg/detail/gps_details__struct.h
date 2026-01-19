// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from custom_msgs:msg/GpsDetails.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__STRUCT_H_
#define CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/GpsDetails in the package custom_msgs.
typedef struct custom_msgs__msg__GpsDetails
{
  double latitude;
  double longitude;
  double altitude;
  uint8_t fix_type;
  uint8_t satellites;
  double horizontal_accuracy;
  double vertical_accuracy;
} custom_msgs__msg__GpsDetails;

// Struct for a sequence of custom_msgs__msg__GpsDetails.
typedef struct custom_msgs__msg__GpsDetails__Sequence
{
  custom_msgs__msg__GpsDetails * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} custom_msgs__msg__GpsDetails__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__STRUCT_H_
