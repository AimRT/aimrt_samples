# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_executable(my_pb_rpc_app)

# Set source file of target
target_sources(my_pb_rpc_app PRIVATE ${src})

# Set include path of target
target_include_directories(
  my_pb_rpc_app
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set link libraries of target
target_link_libraries(
  my_pb_rpc_app
  PRIVATE aimrt::runtime::core
          my_pb_rpc::normal_client_module
          my_pb_rpc::normal_server_module)

if(AIMRT_SAMPLES_INSTALL)
  install(TARGETS my_pb_rpc_app RUNTIME DESTINATION bin)
endif()
