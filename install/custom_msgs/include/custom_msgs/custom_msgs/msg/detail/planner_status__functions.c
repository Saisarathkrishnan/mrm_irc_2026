// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from custom_msgs:msg/PlannerStatus.idl
// generated code does not contain a copyright notice
#include "custom_msgs/msg/detail/planner_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
custom_msgs__msg__PlannerStatus__init(custom_msgs__msg__PlannerStatus * msg)
{
  if (!msg) {
    return false;
  }
  // state
  // nav_mode
  // curr_lat
  // curr_lon
  // goal_lat
  // goal_lon
  // distance_to_goal_m
  // current_yaw_deg
  // target_yaw_deg
  // heading_error_deg
  // cone_detected
  // cone_x
  // cone_y
  // target_cone_id
  // obstacle_detected
  // gps_goal_reached
  // cone_goal_reached
  // autonomous_enabled
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    custom_msgs__msg__PlannerStatus__fini(msg);
    return false;
  }
  return true;
}

void
custom_msgs__msg__PlannerStatus__fini(custom_msgs__msg__PlannerStatus * msg)
{
  if (!msg) {
    return;
  }
  // state
  // nav_mode
  // curr_lat
  // curr_lon
  // goal_lat
  // goal_lon
  // distance_to_goal_m
  // current_yaw_deg
  // target_yaw_deg
  // heading_error_deg
  // cone_detected
  // cone_x
  // cone_y
  // target_cone_id
  // obstacle_detected
  // gps_goal_reached
  // cone_goal_reached
  // autonomous_enabled
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
custom_msgs__msg__PlannerStatus__are_equal(const custom_msgs__msg__PlannerStatus * lhs, const custom_msgs__msg__PlannerStatus * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // state
  if (lhs->state != rhs->state) {
    return false;
  }
  // nav_mode
  if (lhs->nav_mode != rhs->nav_mode) {
    return false;
  }
  // curr_lat
  if (lhs->curr_lat != rhs->curr_lat) {
    return false;
  }
  // curr_lon
  if (lhs->curr_lon != rhs->curr_lon) {
    return false;
  }
  // goal_lat
  if (lhs->goal_lat != rhs->goal_lat) {
    return false;
  }
  // goal_lon
  if (lhs->goal_lon != rhs->goal_lon) {
    return false;
  }
  // distance_to_goal_m
  if (lhs->distance_to_goal_m != rhs->distance_to_goal_m) {
    return false;
  }
  // current_yaw_deg
  if (lhs->current_yaw_deg != rhs->current_yaw_deg) {
    return false;
  }
  // target_yaw_deg
  if (lhs->target_yaw_deg != rhs->target_yaw_deg) {
    return false;
  }
  // heading_error_deg
  if (lhs->heading_error_deg != rhs->heading_error_deg) {
    return false;
  }
  // cone_detected
  if (lhs->cone_detected != rhs->cone_detected) {
    return false;
  }
  // cone_x
  if (lhs->cone_x != rhs->cone_x) {
    return false;
  }
  // cone_y
  if (lhs->cone_y != rhs->cone_y) {
    return false;
  }
  // target_cone_id
  if (lhs->target_cone_id != rhs->target_cone_id) {
    return false;
  }
  // obstacle_detected
  if (lhs->obstacle_detected != rhs->obstacle_detected) {
    return false;
  }
  // gps_goal_reached
  if (lhs->gps_goal_reached != rhs->gps_goal_reached) {
    return false;
  }
  // cone_goal_reached
  if (lhs->cone_goal_reached != rhs->cone_goal_reached) {
    return false;
  }
  // autonomous_enabled
  if (lhs->autonomous_enabled != rhs->autonomous_enabled) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  return true;
}

bool
custom_msgs__msg__PlannerStatus__copy(
  const custom_msgs__msg__PlannerStatus * input,
  custom_msgs__msg__PlannerStatus * output)
{
  if (!input || !output) {
    return false;
  }
  // state
  output->state = input->state;
  // nav_mode
  output->nav_mode = input->nav_mode;
  // curr_lat
  output->curr_lat = input->curr_lat;
  // curr_lon
  output->curr_lon = input->curr_lon;
  // goal_lat
  output->goal_lat = input->goal_lat;
  // goal_lon
  output->goal_lon = input->goal_lon;
  // distance_to_goal_m
  output->distance_to_goal_m = input->distance_to_goal_m;
  // current_yaw_deg
  output->current_yaw_deg = input->current_yaw_deg;
  // target_yaw_deg
  output->target_yaw_deg = input->target_yaw_deg;
  // heading_error_deg
  output->heading_error_deg = input->heading_error_deg;
  // cone_detected
  output->cone_detected = input->cone_detected;
  // cone_x
  output->cone_x = input->cone_x;
  // cone_y
  output->cone_y = input->cone_y;
  // target_cone_id
  output->target_cone_id = input->target_cone_id;
  // obstacle_detected
  output->obstacle_detected = input->obstacle_detected;
  // gps_goal_reached
  output->gps_goal_reached = input->gps_goal_reached;
  // cone_goal_reached
  output->cone_goal_reached = input->cone_goal_reached;
  // autonomous_enabled
  output->autonomous_enabled = input->autonomous_enabled;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

custom_msgs__msg__PlannerStatus *
custom_msgs__msg__PlannerStatus__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__PlannerStatus * msg = (custom_msgs__msg__PlannerStatus *)allocator.allocate(sizeof(custom_msgs__msg__PlannerStatus), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(custom_msgs__msg__PlannerStatus));
  bool success = custom_msgs__msg__PlannerStatus__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
custom_msgs__msg__PlannerStatus__destroy(custom_msgs__msg__PlannerStatus * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    custom_msgs__msg__PlannerStatus__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
custom_msgs__msg__PlannerStatus__Sequence__init(custom_msgs__msg__PlannerStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__PlannerStatus * data = NULL;

  if (size) {
    data = (custom_msgs__msg__PlannerStatus *)allocator.zero_allocate(size, sizeof(custom_msgs__msg__PlannerStatus), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = custom_msgs__msg__PlannerStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        custom_msgs__msg__PlannerStatus__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
custom_msgs__msg__PlannerStatus__Sequence__fini(custom_msgs__msg__PlannerStatus__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      custom_msgs__msg__PlannerStatus__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

custom_msgs__msg__PlannerStatus__Sequence *
custom_msgs__msg__PlannerStatus__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__PlannerStatus__Sequence * array = (custom_msgs__msg__PlannerStatus__Sequence *)allocator.allocate(sizeof(custom_msgs__msg__PlannerStatus__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = custom_msgs__msg__PlannerStatus__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
custom_msgs__msg__PlannerStatus__Sequence__destroy(custom_msgs__msg__PlannerStatus__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    custom_msgs__msg__PlannerStatus__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
custom_msgs__msg__PlannerStatus__Sequence__are_equal(const custom_msgs__msg__PlannerStatus__Sequence * lhs, const custom_msgs__msg__PlannerStatus__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!custom_msgs__msg__PlannerStatus__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
custom_msgs__msg__PlannerStatus__Sequence__copy(
  const custom_msgs__msg__PlannerStatus__Sequence * input,
  custom_msgs__msg__PlannerStatus__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(custom_msgs__msg__PlannerStatus);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    custom_msgs__msg__PlannerStatus * data =
      (custom_msgs__msg__PlannerStatus *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!custom_msgs__msg__PlannerStatus__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          custom_msgs__msg__PlannerStatus__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!custom_msgs__msg__PlannerStatus__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
