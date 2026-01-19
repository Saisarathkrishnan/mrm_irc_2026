from setuptools import find_packages, setup

package_name = 'ex_imu'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ai_sai',
    maintainer_email='ai_sai@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'ex_imu=ex_imu.imu_node:main',
            'rm_auto=ex_imu.imu_node_rm:main',
            'ex_imu_quat=ex_imu.imu_quat:main',
        ],
    },
)
