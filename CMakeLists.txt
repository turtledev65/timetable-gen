# Project
cmake_minimum_required(VERSION 3.10)
project(timetable-gen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output options
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create executable
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
include_directories(src/)
