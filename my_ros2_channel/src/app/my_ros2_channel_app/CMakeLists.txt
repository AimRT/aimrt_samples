# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_executable(my_ros2_channel_app)

# Set source file of target
target_sources(my_ros2_channel_app PRIVATE ${src})

# Set include path of target
target_include_directories(
  my_ros2_channel_app
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set link libraries of target
target_link_libraries(
  my_ros2_channel_app
  PRIVATE aimrt::runtime::core
          my_ros2_channel::normal_publisher_module
          my_ros2_channel::normal_subscriber_module)

if(AIMRT_SAMPLES_INSTALL)
  install(TARGETS my_ros2_channel_app RUNTIME DESTINATION bin)
endif()
