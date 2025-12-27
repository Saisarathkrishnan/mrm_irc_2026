from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node

def generate_launch_description():

    zed_silent = ExecuteProcess(
        cmd=[
            'bash', '-c',
            'ros2 launch zed_wrapper zed_camera.launch.py '
            'camera_model:=zed2i '
            'ros_params_override_path:=/home/mrmnavjet/zed_ros_ws/src/zed-ros2-wrapper/zed_wrapper/config/zed_override.yaml '
            '> /dev/null 2>&1'
        ]
    )

    inference_engine = Node(
        package='yolo',
        executable='inference_engine',
        name='inference_engine',
        output='screen'
    )

    relay_node = Node(
        package='planner',
        executable='relay_node',
        name='relay_node',
        output='screen'
    )

    imu_conversion_silent = ExecuteProcess(
        cmd=[
            'bash', '-c',
            'ros2 run planner imu_conversion_node > /dev/null 2>&1'
        ]
    )

     local_obstacle_filter_silent = ExecuteProcess(
         cmd=[
             'bash', '-c',
             'ros2 run planner local_obstacle_filter_node > /dev/null 2>&1'
         ]
     )

    gps_rtk_silent = ExecuteProcess(
        cmd=[
            'bash', '-c',
            'ros2 run gps gps_rtk > /dev/null 2>&1'
        ]
    )

    return LaunchDescription([
        zed_silent,
        inference_engine,
        relay_node,
        imu_conversion_silent,
        local_obstacle_filter_silent,
        gps_rtk_silent
    ])
