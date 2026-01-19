// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/GuiCommand.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/gui_command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_GuiCommand_search_skew
{
public:
  explicit Init_GuiCommand_search_skew(::custom_msgs::msg::GuiCommand & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::GuiCommand search_skew(::custom_msgs::msg::GuiCommand::_search_skew_type arg)
  {
    msg_.search_skew = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::GuiCommand msg_;
};

class Init_GuiCommand_set_search_skew
{
public:
  explicit Init_GuiCommand_set_search_skew(::custom_msgs::msg::GuiCommand & msg)
  : msg_(msg)
  {}
  Init_GuiCommand_search_skew set_search_skew(::custom_msgs::msg::GuiCommand::_set_search_skew_type arg)
  {
    msg_.set_search_skew = std::move(arg);
    return Init_GuiCommand_search_skew(msg_);
  }

private:
  ::custom_msgs::msg::GuiCommand msg_;
};

class Init_GuiCommand_target_cone_id
{
public:
  explicit Init_GuiCommand_target_cone_id(::custom_msgs::msg::GuiCommand & msg)
  : msg_(msg)
  {}
  Init_GuiCommand_set_search_skew target_cone_id(::custom_msgs::msg::GuiCommand::_target_cone_id_type arg)
  {
    msg_.target_cone_id = std::move(arg);
    return Init_GuiCommand_set_search_skew(msg_);
  }

private:
  ::custom_msgs::msg::GuiCommand msg_;
};

class Init_GuiCommand_goal_lon
{
public:
  explicit Init_GuiCommand_goal_lon(::custom_msgs::msg::GuiCommand & msg)
  : msg_(msg)
  {}
  Init_GuiCommand_target_cone_id goal_lon(::custom_msgs::msg::GuiCommand::_goal_lon_type arg)
  {
    msg_.goal_lon = std::move(arg);
    return Init_GuiCommand_target_cone_id(msg_);
  }

private:
  ::custom_msgs::msg::GuiCommand msg_;
};

class Init_GuiCommand_goal_lat
{
public:
  explicit Init_GuiCommand_goal_lat(::custom_msgs::msg::GuiCommand & msg)
  : msg_(msg)
  {}
  Init_GuiCommand_goal_lon goal_lat(::custom_msgs::msg::GuiCommand::_goal_lat_type arg)
  {
    msg_.goal_lat = std::move(arg);
    return Init_GuiCommand_goal_lon(msg_);
  }

private:
  ::custom_msgs::msg::GuiCommand msg_;
};

class Init_GuiCommand_nav_mode
{
public:
  Init_GuiCommand_nav_mode()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GuiCommand_goal_lat nav_mode(::custom_msgs::msg::GuiCommand::_nav_mode_type arg)
  {
    msg_.nav_mode = std::move(arg);
    return Init_GuiCommand_goal_lat(msg_);
  }

private:
  ::custom_msgs::msg::GuiCommand msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::GuiCommand>()
{
  return custom_msgs::msg::builder::Init_GuiCommand_nav_mode();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__GUI_COMMAND__BUILDER_HPP_
