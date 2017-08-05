cmake_minimum_required(VERSION 3.8)

project(libfmt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(FMT_SOURCE_FILES
        format.cc
        )

add_library(fmt STATIC ${FMT_SOURCE_FILES})