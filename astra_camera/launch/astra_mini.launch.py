from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python import get_package_share_directory
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
import yaml
import os

def generate_launch_description():
    # config_params = os.path.join(get_package_share_directory('astra_camera'), 'params', 'astra_mini_params.yaml')
    params_file = get_package_share_directory(
        "astra_camera") + "/params/astra_mini_params.yaml"
    with open(params_file, 'r') as file:
        config_params = yaml.safe_load(file)
    ROS_DISTRO = os.getenv('ROS_DISTRO')
    print("Current ROS2 Version: ",ROS_DISTRO)
    if ROS_DISTRO == 'humble' or ROS_DISTRO == 'foxy' or ROS_DISTRO == 'iron':
        container = ComposableNodeContainer(
            name='astra_camera_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='astra_camera',
                    plugin='astra_camera::OBCameraNodeFactory',
                    name='camera',
                    namespace='camera',
                    parameters=[config_params]),
                ComposableNode(
                    package='astra_camera',
                    plugin='astra_camera::PointCloudXyzNode',
                    namespace='camera',
                    name='point_cloud_xyz'),
                ComposableNode(
                    package='astra_camera',
                    plugin='astra_camera::PointCloudXyzrgbNode',
                    namespace='camera',
                    name='point_cloud_xyzrgb')
            ],
            output='screen'
        )
    else:
        container = ComposableNodeContainer(node_name='astra_camera_container', package='rclcpp_components', node_namespace='', node_executable='component_container',
            composable_node_descriptions=[
                ComposableNode(package='astra_camera', node_plugin='astra_camera::OBCameraNodeFactory', node_namespace='camera', node_name='camera', parameters=[config_params]),
                ComposableNode(package='astra_camera', node_plugin='astra_camera::PointCloudXyzNode', node_namespace='camera', node_name='point_cloud_xyz'),
                ComposableNode(package='astra_camera', node_plugin='astra_camera::PointCloudXyzrgbNode', node_namespace='camera', node_name='point_cloud_xyzrgb')
            ],
            output='screen'
        )
    
    return LaunchDescription([container])
