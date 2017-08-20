cmake_minimum_required(VERSION 3.8)

project(libfmt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -std=c++98")

set(FMT_SOURCE_FILES
        format.cc
        #easylogging++.cc
        )

add_library(fmt STATIC ${FMT_SOURCE_FILES})