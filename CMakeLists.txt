cmake_minimum_required(VERSION 3.8)
project(event_scheduler)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(event_scheduler ${SOURCE_FILES})