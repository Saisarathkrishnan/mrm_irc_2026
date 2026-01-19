// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/GpsDetails.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/gps_details__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_GpsDetails_vertical_accuracy
{
public:
  explicit Init_GpsDetails_vertical_accuracy(::custom_msgs::msg::GpsDetails & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::GpsDetails vertical_accuracy(::custom_msgs::msg::GpsDetails::_vertical_accuracy_type arg)
  {
    msg_.vertical_accuracy = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

class Init_GpsDetails_horizontal_accuracy
{
public:
  explicit Init_GpsDetails_horizontal_accuracy(::custom_msgs::msg::GpsDetails & msg)
  : msg_(msg)
  {}
  Init_GpsDetails_vertical_accuracy horizontal_accuracy(::custom_msgs::msg::GpsDetails::_horizontal_accuracy_type arg)
  {
    msg_.horizontal_accuracy = std::move(arg);
    return Init_GpsDetails_vertical_accuracy(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

class Init_GpsDetails_satellites
{
public:
  explicit Init_GpsDetails_satellites(::custom_msgs::msg::GpsDetails & msg)
  : msg_(msg)
  {}
  Init_GpsDetails_horizontal_accuracy satellites(::custom_msgs::msg::GpsDetails::_satellites_type arg)
  {
    msg_.satellites = std::move(arg);
    return Init_GpsDetails_horizontal_accuracy(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

class Init_GpsDetails_fix_type
{
public:
  explicit Init_GpsDetails_fix_type(::custom_msgs::msg::GpsDetails & msg)
  : msg_(msg)
  {}
  Init_GpsDetails_satellites fix_type(::custom_msgs::msg::GpsDetails::_fix_type_type arg)
  {
    msg_.fix_type = std::move(arg);
    return Init_GpsDetails_satellites(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

class Init_GpsDetails_altitude
{
public:
  explicit Init_GpsDetails_altitude(::custom_msgs::msg::GpsDetails & msg)
  : msg_(msg)
  {}
  Init_GpsDetails_fix_type altitude(::custom_msgs::msg::GpsDetails::_altitude_type arg)
  {
    msg_.altitude = std::move(arg);
    return Init_GpsDetails_fix_type(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

class Init_GpsDetails_longitude
{
public:
  explicit Init_GpsDetails_longitude(::custom_msgs::msg::GpsDetails & msg)
  : msg_(msg)
  {}
  Init_GpsDetails_altitude longitude(::custom_msgs::msg::GpsDetails::_longitude_type arg)
  {
    msg_.longitude = std::move(arg);
    return Init_GpsDetails_altitude(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

class Init_GpsDetails_latitude
{
public:
  Init_GpsDetails_latitude()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GpsDetails_longitude latitude(::custom_msgs::msg::GpsDetails::_latitude_type arg)
  {
    msg_.latitude = std::move(arg);
    return Init_GpsDetails_longitude(msg_);
  }

private:
  ::custom_msgs::msg::GpsDetails msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::GpsDetails>()
{
  return custom_msgs::msg::builder::Init_GpsDetails_latitude();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__GPS_DETAILS__BUILDER_HPP_
