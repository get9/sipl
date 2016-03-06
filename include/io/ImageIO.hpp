#pragma once

#ifndef SIPL_IO_IMAGEIO_H
#define SIPL_IO_IMAGEIO_H

#include <string>
#include "io/IOBase.hpp"
#include "io/PgmIO.hpp"
#include "io/PpmIO.hpp"
#include "io/BmpIO.hpp"
#include "matrix/Matrix"

namespace sipl
{

class ImageIO : public IOBase
{
public:
    template <typename T>
    static void read(const char* filename, MatrixX<T>& img)
    {
        ImageIO::read(std::string(filename), img);
    }

    template <typename T>
    static void read(const std::string& filename, MatrixX<T>& img)
    {
        switch (file_type(filename)) {
        case FileType::PGM:
            img = PgmIO::read(filename);
        case FileType::PPM:
            img = PpmIO::read(filename);
        case FileType::BMP:
            img = BmpIO::read(filename);
        case FileType::UNKNOWN:
            throw IOException("Unknown file type: " + filename);
        }
    }

    template <typename T>
    static void write(const MatrixX<T>& img, const char* filename)
    {
        ImageIO::write<T>(filename, img);
    }

    template <typename T>
    static void write(const MatrixX<T>& img, const std::string& filename)
    {
        switch (file_type(filename)) {
        case FileType::PGM:
            PgmIO::write(img, filename);
            break;
        case FileType::PPM:
            PpmIO::write(img, filename);
            break;
        case FileType::BMP:
            BmpIO::write(img, filename);
            break;
        case FileType::UNKNOWN:
            throw IOException("Unknown file type: " + filename);
        }
    }

    static FileType file_type(const std::string& filename)
    {
        auto extstart = filename.rfind('.');
        if (extstart == std::string::npos) {
            throw IOException("Bad file path: " + filename);
        }
        auto extname = filename.substr(extstart + 1);
        if (extname == "pgm") {
            return FileType::PGM;
        } else if (extname == "ppm") {
            return FileType::PPM;
        } else if (extname == "bmp") {
            return FileType::BMP;
        } else {
            return FileType::UNKNOWN;
        }
    }
};
}

#endif
