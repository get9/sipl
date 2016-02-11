cmake_minimum_required(VERSION 3.0)
project(SIPL)

# Let's use C++14
set(CMAKE_CXX_STANDARD 14)

# Set CXX_FLAGS so ninja output is colorized
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")

# Export compile commands for YCM
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Set directories of output files
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

add_executable(sipldemo test.cpp src/io/PGMReader.cpp)
