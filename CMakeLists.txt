cmake_minimum_required(VERSION 3.0)
project(SimpleHttpServer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(WIN32)
    find_library(ws2_32 WS2_32)
    include_directories(${BOOST_DIR} modules)
    link_directories(${BOOST_LIBS_DIR})
elseif(UNIX)
    find_library(pthread PTHREAD)
    include_directories(modules)
    endif()

add_executable(Server main.cpp)

add_subdirectory(modules)

target_link_libraries(Server Modules)