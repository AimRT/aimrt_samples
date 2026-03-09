# Set target name
set(CUR_DIR keyboard_module) 
set(CUR_TARGET_NAME my_robot_keyboard_module)
set(CUR_TARGET_ALIAS_NAME my_robot::keyboard_module)

# Set file collection
file(GLOB_RECURSE head_files ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cc)
list(REMOVE_ITEM src ${test_files})

# Add target
add_library(${CUR_TARGET_NAME} STATIC)
add_library(${CUR_TARGET_ALIAS_NAME} ALIAS ${CUR_TARGET_NAME})

# Set source file of target
target_sources(${CUR_TARGET_NAME} PRIVATE ${src})

# Set include path of target
target_include_directories(
  ${CUR_TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/include>
)

target_link_directories(
  ${CUR_TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib>
)

# Set link libraries of target
find_package(sensor_msgs)
find_package(SDL2 REQUIRED)

target_link_libraries(
  ${CUR_TARGET_NAME}
  PRIVATE yaml-cpp::yaml-cpp
  PUBLIC aimrt::interface::aimrt_module_cpp_interface
         aimrt::interface::aimrt_module_ros2_interface
         sensor_msgs::sensor_msgs__rosidl_generator_cpp
         sensor_msgs::sensor_msgs__rosidl_typesupport_cpp
         sensor_msgs::sensor_msgs__rosidl_typesupport_fastrtps_cpp
         sensor_msgs::sensor_msgs__rosidl_typesupport_introspection_cpp
         robot_control_aimrt_rpc_gencode
         SDL2
         SDL2_ttf
         qpOASES)

# Set test of target
if(XYBER_X1_INFER_BUILD_TESTS AND test_files)
  add_gtest_target(TEST_TARGET ${CUR_TARGET_NAME} TEST_SRC ${test_files})
endif()

# Install config files
install(
  DIRECTORY third_party/lib/
  DESTINATION lib
)
