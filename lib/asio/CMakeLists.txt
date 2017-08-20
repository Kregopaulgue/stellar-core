cmake_minimum_required(VERSION 3.8)

project(asio)

add_definitions(-DASIO_STANDALONE -DNOMINMAX -DASIO_SEPARATE_COMPILATION)
#remove_definitions(-DASIO_HEADER_ONLY)
include_directories(include)
include_directories(include/asio)

set(ASIO_SOURCE_FILES
        src/asio.cpp
        #src/asio_ssl.cpp
        )
add_library(asio STATIC ${ASIO_SOURCE_FILES})