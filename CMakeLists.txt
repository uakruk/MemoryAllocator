cmake_minimum_required(VERSION 3.3)
project(OCP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Allocator.cpp Allocator.h)
add_executable(OCP ${SOURCE_FILES})