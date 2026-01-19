// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from custom_msgs:msg/MarkerTag.idl
// generated code does not contain a copyright notice
#include "custom_msgs/msg/detail/marker_tag__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
custom_msgs__msg__MarkerTag__init(custom_msgs__msg__MarkerTag * msg)
{
  if (!msg) {
    return false;
  }
  // is_found
  // id
  // x
  // y
  return true;
}

void
custom_msgs__msg__MarkerTag__fini(custom_msgs__msg__MarkerTag * msg)
{
  if (!msg) {
    return;
  }
  // is_found
  // id
  // x
  // y
}

bool
custom_msgs__msg__MarkerTag__are_equal(const custom_msgs__msg__MarkerTag * lhs, const custom_msgs__msg__MarkerTag * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // is_found
  if (lhs->is_found != rhs->is_found) {
    return false;
  }
  // id
  if (lhs->id != rhs->id) {
    return false;
  }
  // x
  if (lhs->x != rhs->x) {
    return false;
  }
  // y
  if (lhs->y != rhs->y) {
    return false;
  }
  return true;
}

bool
custom_msgs__msg__MarkerTag__copy(
  const custom_msgs__msg__MarkerTag * input,
  custom_msgs__msg__MarkerTag * output)
{
  if (!input || !output) {
    return false;
  }
  // is_found
  output->is_found = input->is_found;
  // id
  output->id = input->id;
  // x
  output->x = input->x;
  // y
  output->y = input->y;
  return true;
}

custom_msgs__msg__MarkerTag *
custom_msgs__msg__MarkerTag__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__MarkerTag * msg = (custom_msgs__msg__MarkerTag *)allocator.allocate(sizeof(custom_msgs__msg__MarkerTag), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(custom_msgs__msg__MarkerTag));
  bool success = custom_msgs__msg__MarkerTag__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
custom_msgs__msg__MarkerTag__destroy(custom_msgs__msg__MarkerTag * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    custom_msgs__msg__MarkerTag__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
custom_msgs__msg__MarkerTag__Sequence__init(custom_msgs__msg__MarkerTag__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__MarkerTag * data = NULL;

  if (size) {
    data = (custom_msgs__msg__MarkerTag *)allocator.zero_allocate(size, sizeof(custom_msgs__msg__MarkerTag), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = custom_msgs__msg__MarkerTag__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        custom_msgs__msg__MarkerTag__fini(&data[i - 1]);
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
custom_msgs__msg__MarkerTag__Sequence__fini(custom_msgs__msg__MarkerTag__Sequence * array)
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
      custom_msgs__msg__MarkerTag__fini(&array->data[i]);
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

custom_msgs__msg__MarkerTag__Sequence *
custom_msgs__msg__MarkerTag__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  custom_msgs__msg__MarkerTag__Sequence * array = (custom_msgs__msg__MarkerTag__Sequence *)allocator.allocate(sizeof(custom_msgs__msg__MarkerTag__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = custom_msgs__msg__MarkerTag__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
custom_msgs__msg__MarkerTag__Sequence__destroy(custom_msgs__msg__MarkerTag__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    custom_msgs__msg__MarkerTag__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
custom_msgs__msg__MarkerTag__Sequence__are_equal(const custom_msgs__msg__MarkerTag__Sequence * lhs, const custom_msgs__msg__MarkerTag__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!custom_msgs__msg__MarkerTag__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
custom_msgs__msg__MarkerTag__Sequence__copy(
  const custom_msgs__msg__MarkerTag__Sequence * input,
  custom_msgs__msg__MarkerTag__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(custom_msgs__msg__MarkerTag);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    custom_msgs__msg__MarkerTag * data =
      (custom_msgs__msg__MarkerTag *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!custom_msgs__msg__MarkerTag__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          custom_msgs__msg__MarkerTag__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!custom_msgs__msg__MarkerTag__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
