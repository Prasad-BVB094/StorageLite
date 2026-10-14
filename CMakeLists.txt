cmake_minimum_required(VERSION 3.10)
project(StorageLite C)

# Set C standard and compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /Zi") # Warnings as errors, debug symbols
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -g")
endif()

# Specify include directories
include_directories(include)

# Find all source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(storagelite ${SOURCES})
