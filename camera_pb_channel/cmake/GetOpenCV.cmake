# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

include(FetchContent)

message(STATUS "get opencv ...")

set(opencv_DOWNLOAD_URL
    "https://github.com/opencv/opencv/archive/4.8.0.tar.gz"
    CACHE STRING "")

if(opencv_LOCAL_SOURCE)
  FetchContent_Declare(
    opencv
    SOURCE_DIR ${opencv_LOCAL_SOURCE}
    OVERRIDE_FIND_PACKAGE)
else()
  FetchContent_Declare(
    opencv
    URL ${opencv_DOWNLOAD_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    OVERRIDE_FIND_PACKAGE)
endif()

function(get_opencv)
  FetchContent_GetProperties(opencv)
  if(NOT opencv_POPULATED)

    set(BUILD_opencv_world ON CACHE BOOL "")


    set(BUILD_opencv_core ON CACHE BOOL "")
    set(BUILD_opencv_imgcodecs ON CACHE BOOL "")
    set(BUILD_opencv_videoio ON CACHE BOOL "")
    set(BUILD_opencv_objdetect ON CACHE BOOL "")
    
    # 明确禁用其他模块
    set(BUILD_opencv_python OFF CACHE BOOL "")
    set(BUILD_opencv_dnn OFF CACHE BOOL "")
    set(BUILD_opencv_ml OFF CACHE BOOL "")
    set(BUILD_opencv_photo OFF CACHE BOOL "")
    set(BUILD_opencv_video OFF CACHE BOOL "")
    set(BUILD_opencv_gapi OFF CACHE BOOL "")
    
    # 禁用其他功能
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
    set(BUILD_TESTS OFF CACHE BOOL "")
    set(BUILD_JAVA OFF CACHE BOOL "")
    set(WITH_PROTOBUF OFF CACHE BOOL "")
    set(WITH_VTK OFF CACHE BOOL "")

    FetchContent_MakeAvailable(opencv)
    include_directories(${opencv_SOURCE_DIR}/include)
  endif()
endfunction()

get_opencv()