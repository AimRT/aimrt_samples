# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(helloworld_module STATIC)
add_library(helloworld::helloworld_module ALIAS helloworld_module)

# Set source file of target
target_sources(helloworld_module PRIVATE ${src})

# Set include path of target
target_include_directories(
  helloworld_module
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Set link libraries of target
target_link_libraries(
  helloworld_module
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface)
