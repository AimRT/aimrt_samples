# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

add_subdirectory(protocols)

add_subdirectory(module/camera_module)
add_subdirectory(module/image_recognition_module)

add_subdirectory(app/camera_pb_channel_app)

add_custom_target(
  ${PROJECT_NAME}_build_all ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/install/bin ${CMAKE_BINARY_DIR}
  DEPENDS camera_pb_channel_app)

# Install
if(AIMRT_SAMPLES_INSTALL)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/bin
    DESTINATION .
    USE_SOURCE_PERMISSIONS)
endif()
