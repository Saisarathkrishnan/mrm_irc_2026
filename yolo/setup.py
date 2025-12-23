from setuptools import setup
from glob import glob
import os

package_name = 'yolo'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],

    data_files=[
        ('share/ament_index/resource_index/packages',
         ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name), ['yolo/cone_v1.engine']),
    ],

    install_requires=['setuptools'],
    zip_safe=True,

    maintainer='vighneshreddy',
    maintainer_email='example@email.com',
    description='TensorRT YOLOv8 cone detection node for Jetson Orin (ROS 2 Humble)',
    license='MIT',

    entry_points={
        'console_scripts': [
            'inference = yolo.inference_node:main',
            'inference_engine = yolo.inference_engine:main',
        ],
    },
)
