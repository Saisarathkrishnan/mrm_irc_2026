// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from custom_msgs:msg/ArmPwm.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__STRUCT_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__custom_msgs__msg__ArmPwm __attribute__((deprecated))
#else
# define DEPRECATED__custom_msgs__msg__ArmPwm __declspec(deprecated)
#endif

namespace custom_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ArmPwm_
{
  using Type = ArmPwm_<ContainerAllocator>;

  explicit ArmPwm_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->link1 = 0ll;
      this->link2 = 0ll;
      this->swivel = 0ll;
      this->gripper = 0ll;
    }
  }

  explicit ArmPwm_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->link1 = 0ll;
      this->link2 = 0ll;
      this->swivel = 0ll;
      this->gripper = 0ll;
    }
  }

  // field types and members
  using _link1_type =
    int64_t;
  _link1_type link1;
  using _link2_type =
    int64_t;
  _link2_type link2;
  using _swivel_type =
    int64_t;
  _swivel_type swivel;
  using _gripper_type =
    int64_t;
  _gripper_type gripper;

  // setters for named parameter idiom
  Type & set__link1(
    const int64_t & _arg)
  {
    this->link1 = _arg;
    return *this;
  }
  Type & set__link2(
    const int64_t & _arg)
  {
    this->link2 = _arg;
    return *this;
  }
  Type & set__swivel(
    const int64_t & _arg)
  {
    this->swivel = _arg;
    return *this;
  }
  Type & set__gripper(
    const int64_t & _arg)
  {
    this->gripper = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    custom_msgs::msg::ArmPwm_<ContainerAllocator> *;
  using ConstRawPtr =
    const custom_msgs::msg::ArmPwm_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::ArmPwm_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      custom_msgs::msg::ArmPwm_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__custom_msgs__msg__ArmPwm
    std::shared_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__custom_msgs__msg__ArmPwm
    std::shared_ptr<custom_msgs::msg::ArmPwm_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ArmPwm_ & other) const
  {
    if (this->link1 != other.link1) {
      return false;
    }
    if (this->link2 != other.link2) {
      return false;
    }
    if (this->swivel != other.swivel) {
      return false;
    }
    if (this->gripper != other.gripper) {
      return false;
    }
    return true;
  }
  bool operator!=(const ArmPwm_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ArmPwm_

// alias to use template instance with default allocator
using ArmPwm =
  custom_msgs::msg::ArmPwm_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__ARM_PWM__STRUCT_HPP_
