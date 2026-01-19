from setuptools import setup
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
        (os.path.join('share', package_name, 'models'),
         ['yolo/cone_v1.engine']),
    ],

    install_requires=['setuptools'],
    zip_safe=True,

    maintainer='vighneshreddy',
    maintainer_email='example@email.com',
    description='TensorRT YOLOv8 cone detection nodes (single + tiled) with Flask streaming',
    license='MIT',

    entry_points={
        'console_scripts': [
            'inference = yolo.inference_node:main',
            'inference_engine = yolo.inference_engine:main',
            'inference_engine_tiled = yolo.inference_engine_tiled:main',
        ],
    },
)
