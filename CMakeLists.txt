cmake_minimum_required(VERSION 2.8)
project(WebServer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

aux_source_directory(. DIR_SRCS)

find_package(Threads)

add_executable(WebServer ${DIR_SRCS} main.cpp)

target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})
