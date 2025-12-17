# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

add_subdirectory(protocols)

add_subdirectory(module/hal_camera_module)
add_subdirectory(module/human_detector_module)

add_subdirectory(pkg/human_detector_pkg)

add_custom_target(
  ${PROJECT_NAME}_build_all ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/install/bin ${CMAKE_BINARY_DIR}
  DEPENDS aimrt::runtime::main
          human_detector_pkg)

# # Install
# if(AIMRT_MUJOCO_SIM_INSTALL)
#   install(
#     DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/bin
#     DESTINATION .
#     USE_SOURCE_PERMISSIONS)
# endif()
