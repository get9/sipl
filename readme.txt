Simple Image Processing Library

This library is the start of a very simple, basic image processing library. The
main files are all included in both the include/ and src/ directories. Under
include/ you can find the template header files that instantiate many of the
types used throughout - mainly the matrix/vector types. Under the src/ directory
you can find implementations of the I/O routines (for PGM and PPM).


Code Layout
There are a few modules in the source:

* io
    - Handles all I/O functionality (PPM/PGM)
* matrix
    - Handles matrix math
    - Different types like Vectors, fixed-sized and dynamic-sized matrices
* improc
    - Image processing routines
    - convert_to_grayscale(), projective_transform live here


Usage
The usage of the program is as described in the problem statement:

    HW1 -i inputFileName -o outputFileName [-c|{-p transformFileName N|B}]

where:

    -i: the input file name
    -o: the output file name
    -c: convert input file to grayscale (only works for .ppm files)
    -p: transformation matrix file used to transform input file
    [N/B]: Nearest neighbor interpolation/Bilinear interpolation for
           transformation
