cmake_minimum_required(VERSION 3.10)

project(Matching_Engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES "*cpp")

add_executable(matching_engine ${SOURCES})
