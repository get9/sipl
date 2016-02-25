#pragma once

#ifndef SIPL_IO_PPMIO_H
#define SIPL_IO_PPMIO_H

#include "cassert"
#include "io/IOBase.hpp"
#include "io/NetpbmIOBase.hpp"
#include "matrix/Matrix.hpp"
#include "matrix/Vector.hpp"

namespace sipl
{
class PpmIO : public NetpbmIOBase
{
public:
    PpmIO() = default;

    // Reads
    static MatrixX<RgbPixel> read(const char* filename);
    static MatrixX<RgbPixel> read(const std::string& filename);

    // Writes
    static void write(const MatrixX<RgbPixel>& mat,
                      const char* filename,
                      const FileType type = FileType::BINARY);
    static void write(const MatrixX<RgbPixel>& mat,
                      const std::string& filename,
                      const FileType type = FileType::BINARY);

private:
    // look at magic number to determine file type
    static FileType determine_file_type(const std::string& filename);

    // Read a binary file
    static MatrixX<RgbPixel> read_binary(const std::string& filename);

    // Read an ascii file
    static MatrixX<RgbPixel> read_ascii(const std::string& filename);

    // Write binary file
    static void write_binary(const MatrixX<RgbPixel>& mat,
                             const std::string& filename);

    // Write binary file
    static void write_ascii(const MatrixX<RgbPixel>& mat,
                            const std::string& filename);
};
}

#endif
