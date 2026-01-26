from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
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
        )
    )

    relay_node = Node(package='planner',executable='relay_node',output='screen')
    yolo_node = Node(package='yolo',executable='inference',output='screen')
    imu_node = Node(package='planner',executable='imu_conversion_node',output='screen')
    gps_node = Node(package='gps',executable='gps_rtk',output='screen')

    return LaunchDescription([
        zed_launch,
        relay_node,
        yolo_node,
        imu_node,
        gps_node
    ])
