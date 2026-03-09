cmake_minimum_required(VERSION 3.24)

project(xyber_x1_infer LANGUAGES C CXX)

# Set cmake path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Some option
option(XYBER_X1_INFER_BUILD_TESTS "Build test" OFF)
option(XYBER_X1_INFER_SIMULATION "Build simulation" OFF)

# Some necessary settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(INSTALL_CONFIG_NAME ${PROJECT_NAME}-config)

# Master project configuration
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_CXX_VISIBLITY_INLINES_HIDDEN ON)
  set(BUILD_SHARED_LIBS OFF)

  if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g")

    if(XYBER_X1_INFER_BUILD_TESTS)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    endif()
  endif()

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Include cmake module
include(GetAimRT)
include(GetAimrtMujocoSim)
include(GetSDL2)

find_package(rclcpp REQUIRED)

# Add subdirectory
add_subdirectory(src)

# Install
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  DESTINATION ./
  FILES_MATCHING
  PATTERN "*.cmake")
install(EXPORT ${INSTALL_CONFIG_NAME} DESTINATION lib/cmake/${PROJECT_NAME})
