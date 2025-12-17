# Set target name
set(CUR_TARGET_NAME "helloworld_pkg")

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(${CUR_TARGET_NAME} SHARED)
add_library(helloworld::${CUR_TARGET_NAME} ALIAS ${CUR_TARGET_NAME})

# Set source file of target
target_sources(${CUR_TARGET_NAME} PRIVATE ${src})

# Set link libraries of target
target_link_libraries(
  ${CUR_TARGET_NAME}
  PRIVATE aimrt::interface::aimrt_pkg_c_interface
          helloworld::helloworld_module)
