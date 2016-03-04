cmake_minimum_required(VERSION 3.0)
project(SIPL)

# Let's use C++14
set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O1")

# Set CXX_FLAGS so ninja output is colorized
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
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
    add_compile_options(-Werror)
endif()

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
    src/io/BmpIO.cpp
)

#add_executable(HW2histo histogram.cpp "${SIPL_SOURCES}")
#add_executable(HW2filter filter_test.cpp "${SIPL_SOURCES}")
#add_executable(HW1 test.cpp "${SIPL_SOURCES}")
#add_executable(vectortest vector_test.cpp)# "${SIPL_SOURCES}")
add_executable(matrixtest matrix_test.cpp)# "${SIPL_SOURCES}")
