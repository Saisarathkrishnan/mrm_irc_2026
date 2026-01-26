cmake_minimum_required(VERSION 3.8)
project(planner)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(custom_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
)

add_library(shared_state src/shared_state.cpp)
target_include_directories(shared_state PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(shared_state
  rclcpp
  std_msgs
  custom_msgs
)

add_executable(planner_node
  src/main.cpp
  src/irc_planner.cpp
)
ament_target_dependencies(planner_node
  rclcpp
  std_msgs
  sensor_msgs
  geometry_msgs
  custom_msgs
  nav_msgs
  std_srvs
  tf2
  tf2_geometry_msgs
  pcl_conversions
)
target_link_libraries(planner_node
  shared_state
  ${PCL_LIBRARIES}
)

add_executable(relay_node src/relay_node.cpp)
ament_target_dependencies(relay_node
  rclcpp
  geometry_msgs
  std_msgs
  std_srvs
)
target_link_libraries(relay_node shared_state)

add_executable(imu_conversion_node src/imu_conversion.cpp)
ament_target_dependencies(imu_conversion_node
  rclcpp
  sensor_msgs
  geometry_msgs
  tf2
  tf2_geometry_msgs
  custom_msgs
)
target_link_libraries(imu_conversion_node shared_state)

add_executable(motorcontroller src/motorcontroller.cpp)
ament_target_dependencies(motorcontroller
  rclcpp
  geometry_msgs
  std_msgs
)
target_link_libraries(motorcontroller shared_state)

add_executable(obstacle_presence_logger src/obstacle_presence_logger.cpp)
ament_target_dependencies(obstacle_presence_logger
  rclcpp
  sensor_msgs
)

add_executable(move_forward src/move_forward.cpp)
ament_target_dependencies(move_forward
  rclcpp
  geometry_msgs
)

add_executable(move_backward src/move_backward.cpp)
ament_target_dependencies(move_backward
  rclcpp
  geometry_msgs
)

add_executable(move_left src/move_left.cpp)
ament_target_dependencies(move_left
  rclcpp
  geometry_msgs
)

add_executable(move_right src/move_right.cpp)
ament_target_dependencies(move_right
  rclcpp
  geometry_msgs
)

add_executable(hill_att src/hill_att.cpp)
ament_target_dependencies(hill_att
  rclcpp
  sensor_msgs
  geometry_msgs
  custom_msgs
)

install(TARGETS
  shared_state
  planner_node
  relay_node
  imu_conversion_node
  motorcontroller
  obstacle_presence_logger
  move_forward
  move_backward
  move_left
  move_right
  hill_att
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(PROGRAMS src/mode_switch.py DESTINATION lib/${PROJECT_NAME})

ament_package()
