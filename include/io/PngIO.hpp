#pragma once

#ifndef SIPL_IO_PNGIO_H
#define SIPL_IO_PNGIO_H

#include "io/IOBase.hpp"
#include "lodepng.h"
#include "matrix/Matrix"
#include <string>

namespace sipl
{

class PngIO : public IOBase
{
public:
    static MatrixX<RgbPixel> read(const char* filename);

    static MatrixX<RgbPixel> read(const std::string& filename);

    static void write(const MatrixX<RgbPixel>& mat, const char* filename);

    static void write(const MatrixX<RgbPixel>& mat,
                      const std::string& filename);
};
}

#endif
