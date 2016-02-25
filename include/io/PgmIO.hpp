#pragma once

#ifndef SIPL_IO_PGMIO_H
#define SIPL_IO_PGMIO_H

#include "matrix/Matrix.hpp"
#include "io/IOBase.hpp"
#include "io/NetpbmIOBase.hpp"

namespace sipl
{
class PgmIO : public NetpbmIOBase
{
public:
    PgmIO() = default;

    // Reading
    static MatrixX<uint8_t> read(const char* filename);

    static MatrixX<uint8_t> read(const std::string& filename);

    // Writing
    static void write(const MatrixX<uint8_t>& mat,
                      const char* filename,
                      const FileType type = FileType::BINARY);

    static void write(const MatrixX<uint8_t>& mat,
                      const std::string& filename,
                      const FileType type = FileType::BINARY);

private:
    // look at magic number to determine file type
    static FileType determine_file_type(const std::string& filename);

    // Read a binary file
    static MatrixX<uint8_t> read_binary(const std::string& filename);

    // Read an ascii file
    static MatrixX<uint8_t> read_ascii(const std::string& filename);

    // Write binary file
    static void write_binary(const MatrixX<uint8_t>& mat,
                             const std::string& filename);

    // Write binary file
    static void write_ascii(const MatrixX<uint8_t>& mat,
                            const std::string& filename);
};
}

#endif
