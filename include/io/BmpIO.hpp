#pragma once

// In order to get valid data from the BMP file, need to pack to nearest 2-byte
// boundary, not 4
#pragma pack(2)

#ifndef SIPL_IO_BMPIO_H
#define SIPL_IO_BMPIO_H

#include "matrix/Matrix.hpp"
#include "io/IOBase.hpp"

namespace sipl
{

// A simple BMP reader/writer. It only handles 8-bit grayscale so far
class BmpIO : public IOBase
{
public:
    // Obtained from:
    // http://stackoverflow.com/a/11744950
    // These are windows #defines
    using LONG = int32_t;
    using WORD = uint16_t;
    using DWORD = uint32_t;

    struct BITMAPFILEHEADER {
        WORD bfType;
        DWORD bfSize;
        WORD bfReserved1;
        WORD bfReserved2;
        DWORD bfOffBits;
    };

    struct BITMAPINFOHEADER {
        DWORD biSize;
        LONG biWidth;
        LONG biHeight;
        WORD biPlanes;
        WORD biBitCount;
        DWORD biCompression;
        DWORD biSizeImage;
        LONG biXPelsPerMeter;
        LONG biYPelsPerMeter;
        DWORD biClrUsed;
        DWORD biClrImportant;
    };

    // Reading
    static MatrixX<uint8_t> read(const char* filename);

    static MatrixX<uint8_t> read(const std::string& filename);

    // Writing
    static void write(const MatrixX<uint8_t>& mat, const char* filename);

    static void write(const MatrixX<uint8_t>& mat, const std::string& filename);
};
}

#endif
