cmake_minimum_required(VERSION 3.8)

project(libsoci_sqlite3)

include_directories(../sqlite)
include_directories(../soci/src/core)


set(SOURCE_SOCI_SQLITE_FILES
        ../soci/src/backends/sqlite3/blob.cpp
        ../soci/src/backends/sqlite3/common.cpp
        ../soci/src/backends/sqlite3/factory.cpp
        ../soci/src/backends/sqlite3/row-id.cpp
        ../soci/src/backends/sqlite3/session.cpp
        ../soci/src/backends/sqlite3/standard-into-type.cpp
        ../soci/src/backends/sqlite3/standard-use-type.cpp
        ../soci/src/backends/sqlite3/statement.cpp
        ../soci/src/backends/sqlite3/vector-into-type.cpp
        ../soci/src/backends/sqlite3/vector-use-type.cpp
        )

add_library(soci_sqlite3 STATIC ${SOURCE_SOCI_SQLITE_FILES})

target_link_libraries(soci_sqlite3 soci)
target_link_libraries(soci_sqlite3 sqlite)
