cmake_minimum_required(VERSION 3.5)
project(custom_msgs)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(geometry_msgs REQUIRED)

set(msg_files
  "msg/MarkerTag.msg"
  "msg/ImuData.msg"
  "msg/GuiCommand.msg"
  "msg/PlannerStatus.msg"
  "msg/GpsDetails.msg"
  "msg/ArmPwm.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES geometry_msgs
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
