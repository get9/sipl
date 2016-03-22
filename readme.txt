================================================================================
Simple Image Processing Library
================================================================================

This library is the start of a very simple, basic image processing library. The
main files are all included in both the include/ and src/ directories. Under
include/ you can find the template header files that instantiate many of the
types used throughout - mainly the matrix/vector types. Under the src/ directory
you can find implementations of the I/O routines (for PGM, PPM, and BMP).


================================================================================
Building the project
If you would like to build the project, it uses a CMake-based project structure.
Assuming you have CMake installed, you can run the following in a terminal:

    mkdir build && cd build
    cmake -G"Visual Studio 14 2015" ..

which will then generate all the Visual Studio-specific files and generate the
correct build targets. You can then open the project in VS.

See here: https://cmake.org/cmake/help/v3.5/manual/cmake-generators.7.html
for a list of all the generators available and you can select your specific
VS version.


================================================================================
Code Layout
There are a few modules in the source:

* io
    - Handles all I/O functionality (PPM/PGM/BMP)
* matrix
    - Handles matrix math
    - Different types like Vectors, fixed-sized and dynamic-sized matrices
* improc
    - Image processing routines
    - Convolution/filtering routines are in Improc.hpp
    - convert_to_grayscale(), projective_transform live here
    - Histogram handling features are in Histogram.hpp
* examples
    - Contains all executables for all homeworks thus far:
         * test.cpp - HW1
	 * histogram.cpp - HW2
	 * filter_test.cpp - HW2
	 * edge.cpp - HW4
	 * vector_test.cpp / matrix_test.cpp - test files for verifying matrices
	   and vectors work correctly

================================================================================
Usage
The usage of the program is as described in the problem statement:

    HW4edge.exe -i input.bmp -o output.bmp [-s t | -p t | -c sigma t0 t1 t2]

where:

    -i: the input file name
    -o: the output file name

    -s: sobel filter with threshold t
    -p: prewitt filter with threshold t
    -c: canny edge detection with:
    	* gaussian kernel width sigma according to 2 * |2 * sigma| + 1
	* initial threshold t0
	* low threshold t1
	* high threshold t1


================================================================================
Bugs
The output of the Canny edge detector does not exactly match the reference
images, however total image difference is still < 1%.
