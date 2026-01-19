// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from custom_msgs:msg/GuiCommand.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__STRUCT_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__custom_msgs__msg__GuiCommand __attribute__((deprecated))
#else
# define DEPRECATED__custom_msgs__msg__GuiCommand __declspec(deprecated)
#endif

namespace custom_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct GuiCommand_
{
  using Type = GuiCommand_<ContainerAllocator>;

  explicit GuiCommand_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->nav_mode = 0;
      this->goal_lat = 0.0;
      this->goal_lon = 0.0;
      this->target_cone_id = 0l;
      this->set_search_skew = false;
      this->search_skew = 0;
    }
  }

  explicit GuiCommand_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->nav_mode = 0;
      this->goal_lat = 0.0;
      this->goal_lon = 0.0;
      this->target_cone_id = 0l;
      this->set_search_skew = false;
      this->search_skew = 0;
    }
  }

  // field types and members
  using _nav_mode_type =
    int8_t;
  _nav_mode_type nav_mode;
  using _goal_lat_type =
    double;
  _goal_lat_type goal_lat;
  using _goal_lon_type =
    double;
  _goal_lon_type goal_lon;
  using _target_cone_id_type =
    int32_t;
  _target_cone_id_type target_cone_id;
  using _set_search_skew_type =
    bool;
  _set_search_skew_type set_search_skew;
  using _search_skew_type =
    int8_t;
  _search_skew_type search_skew;

  // setters for named parameter idiom
  Type & set__nav_mode(
    const int8_t & _arg)
  {
    this->nav_mode = _arg;
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
  Type & set__target_cone_id(
    const int32_t & _arg)
  {
    this->target_cone_id = _arg;
    return *this;
  }
  Type & set__set_search_skew(
    const bool & _arg)
  {
    this->set_search_skew = _arg;
    return *this;
  }
  Type & set__search_skew(
    const int8_t & _arg)
  {
    this->search_skew = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    custom_msgs::msg::GuiCommand_<ContainerAllocator> *;
  using ConstRawPtr =
    const custom_msgs::msg::GuiCommand_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::GuiCommand_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::GuiCommand_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__custom_msgs__msg__GuiCommand
    std::shared_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__custom_msgs__msg__GuiCommand
    std::shared_ptr<custom_msgs::msg::GuiCommand_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const GuiCommand_ & other) const
  {
    if (this->nav_mode != other.nav_mode) {
      return false;
    }
    if (this->goal_lat != other.goal_lat) {
      return false;
    }
    if (this->goal_lon != other.goal_lon) {
      return false;
    }
    if (this->target_cone_id != other.target_cone_id) {
      return false;
    }
    if (this->set_search_skew != other.set_search_skew) {
      return false;
    }
    if (this->search_skew != other.search_skew) {
      return false;
    }
    return true;
  }
  bool operator!=(const GuiCommand_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct GuiCommand_

// alias to use template instance with default allocator
using GuiCommand =
  custom_msgs::msg::GuiCommand_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__STRUCT_HPP_
