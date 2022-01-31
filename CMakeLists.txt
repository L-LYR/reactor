cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE 
        ${CMAKE_CURRENT_SOURCE_DIR}/.vcpkg/scripts/buildsystems/vcpkg.cmake
        CACHE STRING 
        "Vcpkg toolchain file")

project(reactor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

add_subdirectory(app/echo_server)