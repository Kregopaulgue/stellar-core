cmake_minimum_required(VERSION 3.8)

project(libsqlite)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(
        -DASIO_SEPARATE_COMPILATION=1
        -DSQLITE_OMIT_LOAD_EXTENSION=1
)

set(SOURCE_LIB sqlite3.c)

set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS}")

add_library(sqlite STATIC ${SOURCE_LIB})