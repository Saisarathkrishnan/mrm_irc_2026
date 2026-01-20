from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    return LaunchDescription([

        Node(
            package='rtabmap_slam',
            executable='rtabmap',
            name='rtabmap',
            output='screen',

            parameters=[{
                # Frames
                'frame_id': 'base_link',
                'odom_frame_id': 'odom',
                'map_frame_id': 'map',

                # RGB + DEPTH (SEPARATE TOPICS)
                'subscribe_rgb': True,
                'subscribe_depth': True,
                'subscribe_rgbd': False,
                'subscribe_imu': False,

                # 🔑 THIS IS CRITICAL
                'approx_sync': True,

                # 🔑 DO NOT USE OPTICAL FRAME
                'rgb_frame_id': 'zed_left_camera_frame',
                'depth_frame_id': 'zed_left_camera_frame',

                # 🔑 TF TOLERANCE (prevents future extrapolation)
                'wait_for_transform': 0.3,
                'tf_delay': 0.1,
                'tf_buffer_size': 30.0,

                # QoS
                'qos_image': 1,
                'qos_camera_info': 1,
                'qos_odom': 1,

                # SLAM behavior (stable, odom-driven)
                'Rtabmap/DetectionRate': '8.0',
                'Rtabmap/TimeThr': '0',
                'RGBD/OptimizeFromGraphEnd': 'true',
                'Mem/IncrementalMemory': 'true',
                'Mem/STMSize': '30',

                # LOCAL GRID (THIS IS WHAT YOU WANT)
                'Grid/Sensor': '1',
                'Grid/3D': 'true',
                'Grid/MapFrameProjection': 'false',
                'Grid/CellSize': '0.05',
                'Grid/RangeMin': '0.4',
                'Grid/RangeMax': '6.0',

                # DITCH / NEGATIVE OBSTACLE FRIENDLY
                'Grid/GroundIsObstacle': 'true',
                'Grid/MaxGroundAngle': '25',
                'Grid/MinGroundHeight': '-0.4',
                'Grid/MaxObstacleHeight': '1.5',

                # IMPORTANT
                'Grid/UnknownSpaceFilled': 'false',

                'use_sim_time': False,
            }],

            remappings=[
                ('rgb/image', '/zed/zed_node/rgb/color/rect/image'),
                ('rgb/camera_info', '/zed/zed_node/rgb/color/rect/camera_info'),
                ('depth/image', '/zed/zed_node/depth/depth_registered'),
                ('odom', '/zed/zed_node/odom'),
            ]
        )
    ])
