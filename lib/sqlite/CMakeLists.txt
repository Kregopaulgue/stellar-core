cmake_minimum_required(VERSION 3.8)

project(libsqlite)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_LIB sqlite3.c)

add_library(sqlite STATIC ${SOURCE_LIB})