from launch import LaunchDescription
from launch.actions import ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    zed_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('zed_wrapper'),
                'launch',
                'zed.launch.py'
            )
        ),
        launch_arguments={'ros_args': '--log-level warn'}.items()
    )

    imu_node = ExecuteProcess(
        cmd=[
            'ros2', 'run', 'planner', 'imu_conversion_node',
            '--ros-args', '--log-level', 'warn'
        ],
        output='screen'
    )

    yolo_node = ExecuteProcess(
        cmd=['ros2', 'run', 'yolo', 'inference_engine'],
        output='screen'
    )

    rtabmap_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('planner'),
                'launch',
                'rtab.launch.py'
            )
        )
    )

    relay_node = ExecuteProcess(
        cmd=['ros2', 'run', 'planner', 'relay_node'],
        output='screen'
    )

    gps_node = Node(
        package='gps',
        executable='gps_rtk',
        name='gps',
        output='screen',
        arguments=['--ros-args', '--log-level', 'warn'],
        respawn=True,
        respawn_delay=5.0
    )

    return LaunchDescription([
        zed_launch,
        imu_node,
        yolo_node,
        rtabmap_launch,
        relay_node,
        gps_node
    ])
