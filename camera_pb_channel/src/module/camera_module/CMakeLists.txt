# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(camera_module STATIC)
add_library(camera_pb_channel::camera_module ALIAS camera_module)

# Set source file of target
target_sources(camera_module PRIVATE ${src})

# Set include path of target
target_include_directories(
  camera_module
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# Set link libraries of target
target_link_libraries(
  camera_module
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface
         aimrt::interface::aimrt_module_protobuf_interface
         opencv_core
         opencv_imgcodecs
         opencv_videoio
         image_pb_gencode)
