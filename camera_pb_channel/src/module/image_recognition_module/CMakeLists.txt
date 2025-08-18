# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(image_recognition_module STATIC)
add_library(camera_pb_channel::image_recognition_module ALIAS image_recognition_module)

# Set source file of target
target_sources(image_recognition_module PRIVATE ${src})

# Set include path of target
target_include_directories(
  image_recognition_module
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# Set link libraries of target
target_link_libraries(
  image_recognition_module
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface
         aimrt::interface::aimrt_module_protobuf_interface
         opencv_core
         opencv_imgcodecs
         opencv_objdetect
         image_pb_gencode)
