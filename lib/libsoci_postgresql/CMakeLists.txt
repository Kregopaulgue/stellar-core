cmake_minimum_required(VERSION 3.8)

project(libsoci_postgresql)

if(NOT ${CMAKE_COMPILER_IS_GNUCXX})
add_definitions(
        /wd4996
        /Gd
)
endif()

include_directories(../soci/src/core)
include_directories(../soci/src/backends/postgresql)
include_directories($ENV{POSTGRES_PATH_INCLUDE})

file(GLOB SOURCE_LIBSOCI_POSTRESQL_FILES "../soci/src/backends/postgresql/*.cpp")

#find_library(PLIBS NAMES PostgreSQL)

#message(${PLIBS})
find_package(PostgreSQL REQUIRED)

#if(P_FOUND)
#set(POSTLS PLIBS)
#link_directories($ENV{POSTGRES_PATH_INCLUDE}/../lib)
#set(CMAKE_STATIC_LINKER_FLAGS /LTCG)

include_directories(${PostgreSQL_INCLUDE_DIR})

add_library(soci_postgresql STATIC ${SOURCE_LIBSOCI_POSTRESQL_FILES})


target_link_libraries(soci_postgresql ${PostgreSQL_LIBRARIES})
target_link_libraries(soci_postgresql soci)
#target_link_libraries(soci_postgresql ${PostgreSQL_LIBRARIES})