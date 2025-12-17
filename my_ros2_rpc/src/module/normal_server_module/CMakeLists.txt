# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(normal_server_module STATIC)
add_library(my_ros2_rpc::normal_server_module ALIAS normal_server_module)

# Set source file of target
target_sources(normal_server_module PRIVATE ${src})

# Set include path of target
target_include_directories(
  normal_server_module
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# Set link libraries of target
target_link_libraries(
  normal_server_module
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface
         aimrt::interface::aimrt_module_ros2_interface
         my_ros2_srvs::my_ros2_srvs__rosidl_generator_cpp # SRV消息类型的ROS库
         my_ros2_srvs::my_ros2_srvs__rosidl_typesupport_cpp
         my_ros2_srvs::my_ros2_srvs__rosidl_typesupport_fastrtps_cpp
         my_ros2_srvs::my_ros2_srvs__rosidl_typesupport_introspection_cpp
         my_ros2_srvs_aimrt_rpc_gencode)
