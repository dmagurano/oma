cmake_minimum_required(VERSION 3.6)
project(Coiote_heuristic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp heuristic.h heuristic.cpp utils.h)
add_custom_target(Coiote ALL COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(Coiote_heuristic ${SOURCE_FILES})