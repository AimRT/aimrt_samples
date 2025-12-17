# ROS2 msg的包名
project("my_ros2_srvs")

# ROS2 MSG生成相关依赖
find_package(rclcpp REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# ROS2 MSG只支持动态库 设置编译动态库模式
set(CUR_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS ON)

# 生成ROS srv
rosidl_generate_interfaces(${PROJECT_NAME} "srv/CalculateSum.srv")

# 设置回项目动态库原来的构建模式
set(BUILD_SHARED_LIBS ${CUR_BUILD_SHARED_LIBS})

# 给msg的target添加命名空间
if(NOT TARGET ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_cpp)
  add_library(${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_cpp ALIAS ${PROJECT_NAME}__rosidl_typesupport_cpp)
endif()

if(NOT TARGET ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp)
  add_library(${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp ALIAS ${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp)
endif()

# 生成 AimRT RPC 桩代码 生成的target名:${PROJECT_NAME}_aimrt_rpc_gencode
add_ros2_aimrt_rpc_gencode_target_for_one_file(
  TARGET_NAME ${PROJECT_NAME}_aimrt_rpc_gencode # 生成的target名
  PACKAGE_NAME ${PROJECT_NAME} # ros2包名
  PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/srv/CalculateSum.srv # ros2 srv文件
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR} # 生成代码保存路径
  DEP_PROTO_TARGETS
    # 依赖库
    rclcpp::rclcpp
    ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_generator_cpp # 依赖ROS2 MSG
    ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_cpp
    ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp
    ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_introspection_cpp)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
