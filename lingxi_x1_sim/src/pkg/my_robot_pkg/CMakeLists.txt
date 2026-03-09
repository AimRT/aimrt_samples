# Set target name
set(CUR_DIR my_robot_pkg)
set(CUR_TARGET_NAME my_robot_my_robot_pkg)
set(CUR_TARGET_ALIAS_NAME my_robot::my_robot_pkg)

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(${CUR_TARGET_NAME} SHARED)
add_library(${CUR_TARGET_ALIAS_NAME} ALIAS ${CUR_TARGET_NAME})

# Set source file of target
target_sources(${CUR_TARGET_NAME} PRIVATE ${src})

# Set link libraries of target
target_link_libraries(
  ${CUR_TARGET_NAME}
  PRIVATE aimrt::interface::aimrt_pkg_c_interface
      my_robot::keyboard_module
      my_robot::control_module)

# Set installation of target
set_property(TARGET ${CUR_TARGET_NAME} PROPERTY EXPORT_NAME ${CUR_TARGET_ALIAS_NAME})
install(
  TARGETS ${CUR_TARGET_NAME}
  EXPORT ${INSTALL_CONFIG_NAME}
  LIBRARY DESTINATION bin)

# Set misc of target
set_target_properties(${CUR_TARGET_NAME} PROPERTIES OUTPUT_NAME ${CUR_DIR})

set_target_properties(${CUR_TARGET_NAME}
    PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
