# Set namespace
set_root_namespace("my_robot")

# protocols
add_subdirectory(protocols)

# module
add_subdirectory(module/keyboard_module)
add_subdirectory(module/control_module)

# pkg
add_subdirectory(pkg/my_robot_pkg)

# install
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CUR_INSTALL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install/linux)
else()
  message(FATAL_ERROR "Unsupport os")
endif()

install(
  DIRECTORY ${CUR_INSTALL_SOURCE_DIR}/bin
  DESTINATION ./
  USE_SOURCE_PERMISSIONS)

# build all
add_custom_target(
  x1_rl_infer_aimrt_build_all ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CUR_INSTALL_SOURCE_DIR}/bin ${CMAKE_BINARY_DIR}
  DEPENDS aimrt::runtime::main
          my_robot::my_robot_pkg)
