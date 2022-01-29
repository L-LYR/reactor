cmake_minimum_required(VERSION 3.22)

project(reactor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

add_subdirectory(app)