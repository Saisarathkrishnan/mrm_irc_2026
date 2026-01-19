// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from custom_msgs:msg/GpsDetails.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__STRUCT_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__custom_msgs__msg__GpsDetails __attribute__((deprecated))
#else
# define DEPRECATED__custom_msgs__msg__GpsDetails __declspec(deprecated)
#endif

namespace custom_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct GpsDetails_
{
  using Type = GpsDetails_<ContainerAllocator>;

  explicit GpsDetails_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->latitude = 0.0;
      this->longitude = 0.0;
      this->altitude = 0.0;
      this->fix_type = 0;
      this->satellites = 0;
      this->horizontal_accuracy = 0.0;
      this->vertical_accuracy = 0.0;
    }
  }

  explicit GpsDetails_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->latitude = 0.0;
      this->longitude = 0.0;
      this->altitude = 0.0;
      this->fix_type = 0;
      this->satellites = 0;
      this->horizontal_accuracy = 0.0;
      this->vertical_accuracy = 0.0;
    }
  }

  // field types and members
  using _latitude_type =
    double;
  _latitude_type latitude;
  using _longitude_type =
    double;
  _longitude_type longitude;
  using _altitude_type =
    double;
  _altitude_type altitude;
  using _fix_type_type =
    uint8_t;
  _fix_type_type fix_type;
  using _satellites_type =
    uint8_t;
  _satellites_type satellites;
  using _horizontal_accuracy_type =
    double;
  _horizontal_accuracy_type horizontal_accuracy;
  using _vertical_accuracy_type =
    double;
  _vertical_accuracy_type vertical_accuracy;

  // setters for named parameter idiom
  Type & set__latitude(
    const double & _arg)
  {
    this->latitude = _arg;
    return *this;
  }
  Type & set__longitude(
    const double & _arg)
  {
    this->longitude = _arg;
    return *this;
  }
  Type & set__altitude(
    const double & _arg)
  {
    this->altitude = _arg;
    return *this;
  }
  Type & set__fix_type(
    const uint8_t & _arg)
  {
    this->fix_type = _arg;
    return *this;
  }
  Type & set__satellites(
    const uint8_t & _arg)
  {
    this->satellites = _arg;
    return *this;
  }
  Type & set__horizontal_accuracy(
    const double & _arg)
  {
    this->horizontal_accuracy = _arg;
    return *this;
  }
  Type & set__vertical_accuracy(
    const double & _arg)
  {
    this->vertical_accuracy = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    custom_msgs::msg::GpsDetails_<ContainerAllocator> *;
  using ConstRawPtr =
    const custom_msgs::msg::GpsDetails_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::GpsDetails_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::GpsDetails_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__custom_msgs__msg__GpsDetails
    std::shared_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__custom_msgs__msg__GpsDetails
    std::shared_ptr<custom_msgs::msg::GpsDetails_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const GpsDetails_ & other) const
  {
    if (this->latitude != other.latitude) {
      return false;
    }
    if (this->longitude != other.longitude) {
      return false;
    }
    if (this->altitude != other.altitude) {
      return false;
    }
    if (this->fix_type != other.fix_type) {
      return false;
    }
    if (this->satellites != other.satellites) {
      return false;
    }
    if (this->horizontal_accuracy != other.horizontal_accuracy) {
      return false;
    }
    if (this->vertical_accuracy != other.vertical_accuracy) {
      return false;
    }
    return true;
  }
  bool operator!=(const GpsDetails_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct GpsDetails_

// alias to use template instance with default allocator
using GpsDetails =
  custom_msgs::msg::GpsDetails_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__STRUCT_HPP_
