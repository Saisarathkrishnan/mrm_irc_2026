cmake_minimum_required(VERSION 3.8)
project(ditch_detection)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

find_package(pcl_conversions REQUIRED)
find_package(PCL REQUIRED)

find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_sensor_msgs REQUIRED)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
)

# ---------- ditch1 ----------
add_executable(ditch1 src/ditch1.cpp)

ament_target_dependencies(ditch1
  rclcpp
  sensor_msgs
  std_msgs
  pcl_conversions
  tf2
  tf2_ros
  tf2_sensor_msgs
)

target_link_libraries(ditch1
  ${PCL_LIBRARIES}
)

# ---------- ditch2 ----------
add_executable(ditch2 src/ditch2.cpp)

ament_target_dependencies(ditch2
  rclcpp
  sensor_msgs
  std_msgs
  pcl_conversions
  tf2
  tf2_ros
  tf2_sensor_msgs
)

target_link_libraries(ditch2
  ${PCL_LIBRARIES}
)

# ---------- ditch3 ----------
add_executable(ditch3 src/ditch3.cpp)

ament_target_dependencies(ditch3
  rclcpp
  sensor_msgs
  std_msgs
  pcl_conversions
  tf2
  tf2_ros
  tf2_sensor_msgs
)

target_link_libraries(ditch3
  ${PCL_LIBRARIES}
)

install(TARGETS
  ditch1
  ditch2
  ditch3
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
