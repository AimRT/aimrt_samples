# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

add_subdirectory(protocols)

add_subdirectory(module/normal_publisher_module)
add_subdirectory(module/normal_subscriber_module)

add_subdirectory(app/my_ros2_channel_app)

add_custom_target(
  ${PROJECT_NAME}_build_all ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/install/bin ${CMAKE_BINARY_DIR}
  DEPENDS my_ros2_channel_app)

# Install
if(AIMRT_SAMPLES_INSTALL)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/bin
    DESTINATION .
    USE_SOURCE_PERMISSIONS)
endif()
