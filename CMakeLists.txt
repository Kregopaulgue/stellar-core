cmake_minimum_required(VERSION 3.8)
project(stellar)

set(CMAKE_CXX_STANDARD 11)

include_directories(libsqlite)

add_subdirectory(src)
add_subdirectory(lib/asio)
add_subdirectory(lib/soci)
add_subdirectory(lib/sqlite)
add_subdirectory(lib/libmedida)
add_subdirectory(lib/libsodium)
add_subdirectory(lib/libsoci_sqlite3)
add_subdirectory(lib/libsoci_postgresql)
add_subdirectory(lib/xdrpp/xdrc)
add_subdirectory(lib/xdrpp/xdrpp)
add_subdirectory(lib/util)
