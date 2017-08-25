find_package(PostgreSQL REQUIRED)

message(${PostgreSQL_LIBRARIES})

target_link_libraries(core ${PostgreSQL_LIBRARIES})
target_link_libraries(core pq)
target_link_libraries(core soci)
target_link_libraries(core 3rdparty)
target_link_libraries(core medida)
target_link_libraries(core xdrpp)
target_link_libraries(core sodium)

#For windows.
if(${CMAKE_HOST_WIN32})
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
    find_library(PSAPI psapi)
    if(NOT ${WSOCK32_LIBRARY})
        message(FATAL_ERROR " WSOCK32_LIBRARY_NOTFOUND")
    endif()
    if(NOT ${WS2_32_LIBRARY})
        message(FATAL_ERROR " WS2_32_LIBRARY_NOTFOUND")
    endif()
    if(NOT ${PSAPI})
        message(FATAL_ERROR " PSAPI_NOTFOUND")
    endif()
    target_link_libraries(core sqlite)
    target_link_libraries(core wsock32 ws2_32)
    target_link_libraries(core psapi)
elseif(${CMAKE_HOST_UNIX})
    #target_link_libraries(core -lm -ldl sqlite)
elseif(${CMAKE_HOST_APPLE})
    message(FATAL_ERROR " NOT FOUND settings libs for APPLE. Please define libs in platform_libs.cmake")
    #define here for APPLE.
elseif(${CMAKE_HOST_SOLARIS})
    message(FATAL_ERROR " NOT FOUND settings libs for SOLARIS. Please define libs in platform_libs.cmake")
    #define here for SOLARIS.
else()
    message(FATAL_ERROR " NOT FOUND settings libs for ${CMAKE_HOST_SYSTEM_NAME}. Please define libs in platform_libs.cmake")
#elseif(${CMAKE_HOST_SYSTEM_NAME} EQUAL "YOUR_HOST_SYSTEM_NAME")
#   ...
#define here for YOUR_SYSTEM.
endif()