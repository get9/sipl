================================================================================
Simple Image Processing Library
================================================================================

This library is the start of a very simple, basic image processing library. The
main files are all included in both the include/ and src/ directories. Under
include/ you can find the template header files that instantiate many of the
types used throughout - mainly the matrix/vector types. Under the src/ directory
you can find implementations of the I/O routines (for PGM and PPM).


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
* filter_test.cpp
    - The main test driver for all filter calculations
* histogram.cpp
    - The main test driver for all histogram calculations


================================================================================
Usage
The usage of the program is as described in the problem statement:

    HW2histo.exe -i input.bmp -o output.bmp [-e|-p|-m matchfile.bmp]

    HW2filter.exe -i input.bmp -o output.bmp -f filter.txt [-m [k]]

where:

    -i: the input file name
    -o: the output file name
    -e: equalize histogram of input file
    -p: plot histogram of input file
    -m: match histogram of inputfile with matchfile

    -f: the filter file name
    -m: median filter, optional 'k' for kth-order filtering


================================================================================
Bugs
For the histogram code, I was unable to match exactly the input files. For
example, the 'tire' image histogram is off by approximately 100 pixels or so.
I believe this is throwing the other histogram calculations off since they
utilize the regular histogram feature as a base for their computation.


================================================================================
Notes
I chose to implement my own Bitmap I/O code. While I can successfully read and
write images (test by doing image subtraction and looking for differences in
pixel values), I'm unsure if it's introducing subtle differences that would
cause my issues with the histogram. However, the filter code produces exactly
the correct output, so I am not convinced it is the I/O code.
