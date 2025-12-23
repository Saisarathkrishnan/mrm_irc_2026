from setuptools import setup, find_packages

package_name = 'gps'

setup(
    name=package_name,
    version='0.0.1',
    packages=find_packages(),
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Your Name',
    maintainer_email='your.email@example.com',
    description='A package for parsing UBlox GPS data',
    license='Apache License 2.0',

    entry_points={
        'console_scripts': [
            'gps_rtcm = gps.gps_rtcm:main',
            'gps_rtk = gps.gps_rtk:main',
        ],
    },

    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
)

