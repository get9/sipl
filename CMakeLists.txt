cmake_minimum_required(VERSION 3.0)
project(SIPL)

# Let's use C++14
set(CMAKE_CXX_STANDARD 14)

# Enable clang Asan
#set(CMAKE_CXX_FLAGS
#    "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O1")

# Set CXX_FLAGS so ninja output is colorized
if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-Wall)
    add_compile_options(-Wpedantic)
    add_compile_options(-Wextra)
    add_compile_options(-Wno-c++98-compat)
    add_compile_options(-fdiagnostics-color=always)
    add_compile_options(-Wno-missing-braces)
    add_compile_options(-Wno-weak-vtables)
    add_compile_options(-Wno-padded)
    add_compile_options(-Wno-sign-conversion)
    add_compile_options(-Wno-c++11-narrowing)
    add_compile_options(-Wimplicit-fallthrough)
    add_compile_options(-Wno-unused-parameter)
    #add_compile_options(-Werror)
    #add_compile_options(-Weverything)
endif()

# Export compile commands for YCM
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Set directories of output files
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Includes
include_directories(include)

# Source files. After adding src subdirectory, SIPL_SOURCES should have all
# sources
add_subdirectory(src)
add_subdirectory(third-party)
include_directories(third-party/lodepng)

# All source files - SIPL and third-party
set(ALL_SOURCES ${SIPL_SOURCES} ${THIRD_PARTY_SOURCES})

#add_executable(HW2histo examples/histogram.cpp ${ALL_SOURCES})
#add_executable(HW2filter examples/filter_test.cpp ${ALL_SOURCES})
#add_executable(HW1 examples/test.cpp ${ALL_SOURCES})
#add_executable(vectortest examples/vector_test.cpp)# ${SIPL_SOURCES})
#add_executable(matrixtest examples/matrix_test.cpp)# ${SIPL_SOURCES})
#add_executable(HW4edge examples/edge.cpp ${ALL_SOURCES})
#add_executable(HW5 examples/pngtest.cpp ${ALL_SOURCES})
add_executable(ndarray_test examples/ndarraytest.cpp ${ALL_SOURCES})
