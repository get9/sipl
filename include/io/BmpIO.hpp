#pragma once

#ifndef SIPL_IO_BMPIO_H
#define SIPL_IO_BMPIO_H

// In order to get valid data from the BMP file, need to pack to nearest 2-byte
// boundary, not 4
#pragma pack(push, 2)

#include <iostream>
#include "matrix/Matrix"
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
    static MatrixXb read(const char* filename);

    static MatrixXb read(const std::string& filename);

    // Writing
    static void write(const MatrixXb& mat, const char* filename);

    static void write(const MatrixXb& mat, const std::string& filename);
};

inline std::ostream& operator<<(std::ostream& s,
                                const BmpIO::BITMAPINFOHEADER& i)
{
    s << "biSize:          " << i.biSize << std::endl;
    s << "biWidth:         " << i.biWidth << std::endl;
    s << "biHeight:        " << i.biHeight << std::endl;
    s << "biPlanes:        " << i.biPlanes << std::endl;
    s << "biBitCount:      " << i.biBitCount << std::endl;
    s << "biCompression:   " << i.biCompression << std::endl;
    s << "biSizeImage:     " << i.biSizeImage << std::endl;
    s << "biXPelsPerMeter: " << i.biXPelsPerMeter << std::endl;
    s << "biYPelsPerMeter: " << i.biYPelsPerMeter << std::endl;
    s << "biClrUsed:       " << i.biClrUsed << std::endl;
    s << "biClrImportant:  " << i.biClrImportant;
    return s;
}

inline std::ostream& operator<<(std::ostream& s,
                                const BmpIO::BITMAPFILEHEADER& h)
{
    const auto bfType = h.bfType;
    char type[] = {static_cast<char>(bfType), static_cast<char>(bfType >> 8),
                   '\0'};
    s << "bfType:          " << type << std::endl;
    s << "bfSize:          " << h.bfSize << std::endl;
    s << "bfReserved1:     " << h.bfReserved1 << std::endl;
    s << "bfReserved2:     " << h.bfReserved2 << std::endl;
    s << "bfOffBits:       " << h.bfOffBits;
    return s;
}
}

// Set alignment back to normal
#pragma pack(pop)

#endif
