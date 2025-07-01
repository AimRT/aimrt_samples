# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(normal_publisher_module STATIC)
add_library(my_pb_channel::normal_publisher_module ALIAS normal_publisher_module)

# Set source file of target
target_sources(normal_publisher_module PRIVATE ${src})

# Set include path of target
target_include_directories(
  normal_publisher_module
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# Set link libraries of target
target_link_libraries(
  normal_publisher_module
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface
         aimrt::interface::aimrt_module_protobuf_interface
         my_pb_channel_pb_gencode)
