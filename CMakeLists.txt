cmake_minimum_required(VERSION 3.10)
project(CppMultithreadBlog
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Header files
file(GLOB_RECURSE HEADERS "include/*.h")

# Add executable
add_executable(CppMultithreadBlog ${SOURCES} ${HEADERS})

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Print build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
