// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from custom_msgs:msg/ArmPwm.idl
// generated code does not contain a copyright notice
#include "custom_msgs/msg/detail/arm_pwm__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
custom_msgs__msg__ArmPwm__init(custom_msgs__msg__ArmPwm * msg)
{
  if (!msg) {
    return false;
  }
  // link1
  // link2
  // swivel
  // gripper
  return true;
}

void
custom_msgs__msg__ArmPwm__fini(custom_msgs__msg__ArmPwm * msg)
{
  if (!msg) {
    return;
  }
  // link1
  // link2
  // swivel
  // gripper
}

bool
custom_msgs__msg__ArmPwm__are_equal(const custom_msgs__msg__ArmPwm * lhs, const custom_msgs__msg__ArmPwm * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // link1
  if (lhs->link1 != rhs->link1) {
    return false;
  }
  // link2
  if (lhs->link2 != rhs->link2) {
    return false;
  }
  // swivel
  if (lhs->swivel != rhs->swivel) {
    return false;
  }
  // gripper
  if (lhs->gripper != rhs->gripper) {
    return false;
  }
  return true;
}

bool
custom_msgs__msg__ArmPwm__copy(
  const custom_msgs__msg__ArmPwm * input,
  custom_msgs__msg__ArmPwm * output)
{
  if (!input || !output) {
    return false;
  }
  // link1
  output->link1 = input->link1;
  // link2
  output->link2 = input->link2;
  // swivel
  output->swivel = input->swivel;
  // gripper
  output->gripper = input->gripper;
  return true;
}

custom_msgs__msg__ArmPwm *
custom_msgs__msg__ArmPwm__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__ArmPwm * msg = (custom_msgs__msg__ArmPwm *)allocator.allocate(sizeof(custom_msgs__msg__ArmPwm), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(custom_msgs__msg__ArmPwm));
  bool success = custom_msgs__msg__ArmPwm__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
custom_msgs__msg__ArmPwm__destroy(custom_msgs__msg__ArmPwm * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    custom_msgs__msg__ArmPwm__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
custom_msgs__msg__ArmPwm__Sequence__init(custom_msgs__msg__ArmPwm__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__ArmPwm * data = NULL;

  if (size) {
    data = (custom_msgs__msg__ArmPwm *)allocator.zero_allocate(size, sizeof(custom_msgs__msg__ArmPwm), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = custom_msgs__msg__ArmPwm__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        custom_msgs__msg__ArmPwm__fini(&data[i - 1]);
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
custom_msgs__msg__ArmPwm__Sequence__fini(custom_msgs__msg__ArmPwm__Sequence * array)
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
      custom_msgs__msg__ArmPwm__fini(&array->data[i]);
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

custom_msgs__msg__ArmPwm__Sequence *
custom_msgs__msg__ArmPwm__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__ArmPwm__Sequence * array = (custom_msgs__msg__ArmPwm__Sequence *)allocator.allocate(sizeof(custom_msgs__msg__ArmPwm__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = custom_msgs__msg__ArmPwm__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
custom_msgs__msg__ArmPwm__Sequence__destroy(custom_msgs__msg__ArmPwm__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    custom_msgs__msg__ArmPwm__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
custom_msgs__msg__ArmPwm__Sequence__are_equal(const custom_msgs__msg__ArmPwm__Sequence * lhs, const custom_msgs__msg__ArmPwm__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!custom_msgs__msg__ArmPwm__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
custom_msgs__msg__ArmPwm__Sequence__copy(
  const custom_msgs__msg__ArmPwm__Sequence * input,
  custom_msgs__msg__ArmPwm__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(custom_msgs__msg__ArmPwm);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    custom_msgs__msg__ArmPwm * data =
      (custom_msgs__msg__ArmPwm *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!custom_msgs__msg__ArmPwm__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          custom_msgs__msg__ArmPwm__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!custom_msgs__msg__ArmPwm__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
