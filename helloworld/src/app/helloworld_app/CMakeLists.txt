# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set file collection
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_executable(helloworld_app)

# Set source file of target
target_sources(helloworld_app PRIVATE ${src})

# Set include path of target
target_include_directories(
  helloworld_app
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set link libraries of target
target_link_libraries(
  helloworld_app
  PRIVATE aimrt::runtime::core
          helloworld::helloworld_module)

if(AIMRT_SAMPLES_INSTALL)
  install(TARGETS helloworld_app RUNTIME DESTINATION bin)
endif()
