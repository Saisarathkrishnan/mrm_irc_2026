from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    return LaunchDescription([

        Node(
            package='rtabmap_slam',
            executable='rtabmap',
            name='rtabmap',
            output='screen',
            arguments=['-d'],
            parameters=[{
                # Frames
                'frame_id': 'base_link',
                'odom_frame_id': 'odom',
                'map_frame_id': 'map',

                # ZED camera frames
                'rgb_frame_id': 'zed_left_camera_frame_optical',
                'depth_frame_id': 'zed_left_camera_frame_optical',

                # Subscriptions
                'subscribe_rgb': True,
                'subscribe_depth': True,
                'subscribe_rgbd': False,
                'subscribe_imu': False,

                # Sync / TF robustness
                'approx_sync': True,
                'wait_for_transform': 0.8,
                'tf_delay': 0.2,
                'sync_queue_size': 20,
                'topic_queue_size': 20,

                # QoS
                'qos_camera_info': 1,

                # RTAB-Map rate control (⚠️ STRING, not float)
                'Rtabmap/DetectionRate': '3.0',
                'Rtabmap/TimeThr': '0',

                # Mapping
                'Rtabmap/PublishOccupancyGrid': True,

                'use_sim_time': False
            }],
            remappings=[
                ('rgb/image', '/zed/zed_node/rgb/color/rect/image'),
                ('rgb/camera_info', '/zed/zed_node/rgb/color/rect/camera_info'),
                ('depth/image', '/zed/zed_node/depth/depth_registered'),
                ('odom', '/zed/zed_node/odom')
            ]
        )
    ])
