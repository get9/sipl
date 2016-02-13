cmake_minimum_required(VERSION 3.0)
project(SIPL)

# Let's use C++14
set(CMAKE_CXX_STANDARD 14)

# Set CXX_FLAGS so ninja output is colorized
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -Weverything -Wno-c++98-compat")
add_compile_options(-fdiagnostics-color=always)
add_compile_options(-Weverything)
add_compile_options(-Wno-c++98-compat)
add_compile_options(-Wno-missing-braces)
add_compile_options(-Wno-weak-vtables)
add_compile_options(-Wno-padded)

# Export compile commands for YCM
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Set directories of output files
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
add_subdirectory(src)

# List of sources
set(SIPL_SOURCES
    src/io/NetpbmIOBase.cpp
    src/io/PgmIO.cpp
    src/io/PpmIO.cpp
)

add_executable(sipldemo test.cpp "${SIPL_SOURCES}")
