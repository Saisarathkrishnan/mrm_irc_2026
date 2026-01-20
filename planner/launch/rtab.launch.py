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

                'rgb_frame_id': 'zed_left_camera_frame_optical',
                'depth_frame_id': 'zed_left_camera_frame_optical',

                # Sync (STRICT)
                'approx_sync': False,          # 🔴 EXACT sync
                'sync_queue_size': 50,
                'wait_for_transform': 3.0,     # 🔴 MAX wait
                'tf_delay': 0.0,
                'tf_tolerance': 0.0,

                # Subscriptions
                'subscribe_rgb': True,
                'subscribe_depth': True,
                'subscribe_rgbd': False,
                'subscribe_imu': False,
                'subscribe_odom': False,

                # RTAB-Map core
                'Rtabmap/DetectionRate': '10.0',  # 🔴 MATCH CAMERA FPS
                'Rtabmap/TimeThr': '0',

                # Grid (LOW LOAD)
                'Grid/3D': 'true',
                'Grid/CellSize': '0.10',
                'Grid/DepthDecimation': '4',
                'Grid/RangeMin': '0.5',
                'Grid/RangeMax': '8.0',
                'Grid/GroundIsObstacle': 'true',

                # Logging
                'log_to_rosout_level': 4,
            }],

            remappings=[
                ('rgb/image', '/zed/zed_node/rgb/color/rect/image'),
                ('rgb/camera_info', '/zed/zed_node/rgb/color/rect/camera_info'),
                ('depth/image', '/zed/zed_node/depth/depth_registered'),
            ]
        )
    ])
