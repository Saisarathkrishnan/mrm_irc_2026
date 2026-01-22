cmake_minimum_required(VERSION 3.8)
project(planner)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------------- Dependencies ----------------
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

find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(PCL REQUIRED COMPONENTS common filters segmentation)
find_package(Eigen3 REQUIRED)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
)

add_definitions(${PCL_DEFINITIONS})

# ---------------- Shared State ----------------
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

# ---------------- Planner ----------------
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
target_link_libraries(planner_node shared_state)

# ---------------- Relay ----------------
add_executable(relay_node src/relay_node.cpp)
ament_target_dependencies(relay_node
  rclcpp
  geometry_msgs
  std_msgs
  std_srvs
)
target_link_libraries(relay_node shared_state)

# ---------------- IMU Conversion ----------------
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

# ---------------- Motor Controller ----------------
add_executable(motorcontroller src/motorcontroller.cpp)
ament_target_dependencies(motorcontroller
  rclcpp
  geometry_msgs
  std_msgs
)
target_link_libraries(motorcontroller shared_state)

# ---------------- WALL NODE (FIXED) ----------------
add_executable(wall_node
  src/wall.cpp
)
ament_target_dependencies(wall_node
  rclcpp
  sensor_msgs
  std_msgs
  pcl_conversions
)
target_link_libraries(wall_node
  ${PCL_LIBRARIES}
)

# ---------------- Inject Node ----------------
add_executable(inject_node src/inject.cpp)
ament_target_dependencies(inject_node
  rclcpp
  sensor_msgs
  pcl_conversions
  cv_bridge
)
target_link_libraries(inject_node
  ${PCL_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

# ---------------- Ditch RANSAC ----------------
add_executable(ditch_ransac_node src/ransac.cpp)
ament_target_dependencies(ditch_ransac_node
  rclcpp
  sensor_msgs
  pcl_conversions
)
target_link_libraries(ditch_ransac_node
  ${PCL_LIBRARIES}
  Eigen3::Eigen
)

# ---------------- Obstacle Logger ----------------
add_executable(obstacle_presence_logger src/obstacle_presence_logger.cpp)
ament_target_dependencies(obstacle_presence_logger
  rclcpp
  sensor_msgs
)

# ---------------- Install ----------------
install(TARGETS
  shared_state
  planner_node
  relay_node
  imu_conversion_node
  motorcontroller
  wall_node              # <<< THIS WAS MISSING / WRONG BEFORE
  inject_node
  ditch_ransac_node
  obstacle_presence_logger
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(PROGRAMS src/mode_switch.py DESTINATION lib/${PROJECT_NAME})

ament_package()
