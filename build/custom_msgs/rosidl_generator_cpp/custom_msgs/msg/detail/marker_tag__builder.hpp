// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/MarkerTag.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__MARKER_TAG__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__MARKER_TAG__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/marker_tag__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_MarkerTag_y
{
public:
  explicit Init_MarkerTag_y(::custom_msgs::msg::MarkerTag & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::MarkerTag y(::custom_msgs::msg::MarkerTag::_y_type arg)
  {
    msg_.y = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::MarkerTag msg_;
};

class Init_MarkerTag_x
{
public:
  explicit Init_MarkerTag_x(::custom_msgs::msg::MarkerTag & msg)
  : msg_(msg)
  {}
  Init_MarkerTag_y x(::custom_msgs::msg::MarkerTag::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_MarkerTag_y(msg_);
  }

private:
  ::custom_msgs::msg::MarkerTag msg_;
};

class Init_MarkerTag_id
{
public:
  explicit Init_MarkerTag_id(::custom_msgs::msg::MarkerTag & msg)
  : msg_(msg)
  {}
  Init_MarkerTag_x id(::custom_msgs::msg::MarkerTag::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_MarkerTag_x(msg_);
  }

private:
  ::custom_msgs::msg::MarkerTag msg_;
};

class Init_MarkerTag_is_found
{
public:
  Init_MarkerTag_is_found()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MarkerTag_id is_found(::custom_msgs::msg::MarkerTag::_is_found_type arg)
  {
    msg_.is_found = std::move(arg);
    return Init_MarkerTag_id(msg_);
  }

private:
  ::custom_msgs::msg::MarkerTag msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::MarkerTag>()
{
  return custom_msgs::msg::builder::Init_MarkerTag_is_found();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__MARKER_TAG__BUILDER_HPP_
