cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD  20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Editor)

add_executable(Editor
        Source/main.cpp)

target_link_libraries(Editor PRIVATE Engine)