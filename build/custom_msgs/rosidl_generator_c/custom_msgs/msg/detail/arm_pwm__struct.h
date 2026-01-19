// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from custom_msgs:msg/ArmPwm.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__STRUCT_H_
#define CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/ArmPwm in the package custom_msgs.
typedef struct custom_msgs__msg__ArmPwm
{
  int64_t link1;
  int64_t link2;
  int64_t swivel;
  int64_t gripper;
} custom_msgs__msg__ArmPwm;

// Struct for a sequence of custom_msgs__msg__ArmPwm.
typedef struct custom_msgs__msg__ArmPwm__Sequence
{
  custom_msgs__msg__ArmPwm * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} custom_msgs__msg__ArmPwm__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__STRUCT_H_
