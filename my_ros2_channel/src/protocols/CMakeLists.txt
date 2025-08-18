# ROS2 msg的包名
project("my_ros2_msgs")

# ROS2 MSG生成相关依赖
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# ROS2 MSG只支持动态库 设置编译动态库模式
set(CUR_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS ON)

# 生成msg
rosidl_generate_interfaces(${PROJECT_NAME} "msg/Header.msg" "msg/Event.msg")

set(BUILD_SHARED_LIBS ${CUR_BUILD_SHARED_LIBS})

# 给msg的target添加命名空间
if(NOT TARGET ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_cpp)
  add_library(${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_cpp ALIAS ${PROJECT_NAME}__rosidl_typesupport_cpp)
endif()

if(NOT TARGET ${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp)
  add_library(${PROJECT_NAME}::${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp ALIAS ${PROJECT_NAME}__rosidl_typesupport_fastrtps_cpp)
endif()

ament_export_dependencies(rosidl_default_runtime)
ament_package()
