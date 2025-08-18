# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(normal_subscriber_module STATIC)
add_library(my_ros2_channel::normal_subscriber_module ALIAS normal_subscriber_module)

# Set source file of target
target_sources(normal_subscriber_module PRIVATE ${src})

# Set include path of target
target_include_directories(
  normal_subscriber_module
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# Set link libraries of target
target_link_libraries(
  normal_subscriber_module
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface
         aimrt::interface::aimrt_module_ros2_interface
         my_ros2_msgs::my_ros2_msgs__rosidl_generator_cpp
         my_ros2_msgs::my_ros2_msgs__rosidl_typesupport_cpp
         my_ros2_msgs::my_ros2_msgs__rosidl_typesupport_fastrtps_cpp
         my_ros2_msgs::my_ros2_msgs__rosidl_typesupport_introspection_cpp)
