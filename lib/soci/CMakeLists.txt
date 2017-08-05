cmake_minimum_required(VERSION 3.8)

project(libsoci)

#add_definitions(-Wno-deprecated)

include_directories(src/core)

file(GLOB SOURCE_SOCI_FILES "src/core/*.cpp")
set(HEADER_SOCI_FILES src/core/*.h)

add_library(soci STATIC ${SOURCE_SOCI_FILES})