# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_executable(camera_pb_channel_app)

# Set source file of target
target_sources(camera_pb_channel_app PRIVATE ${src})

# Set include path of target
target_include_directories(
  camera_pb_channel_app
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set link libraries of target
target_link_libraries(
  camera_pb_channel_app
  PRIVATE aimrt::runtime::core
          camera_pb_channel::image_recognition_module
          camera_pb_channel::camera_module)

if(AIMRT_SAMPLES_INSTALL)
  install(TARGETS camera_pb_channel_app RUNTIME DESTINATION bin)
endif()
