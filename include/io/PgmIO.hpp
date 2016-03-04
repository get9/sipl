#pragma once

#ifndef SIPL_IO_PGMIO_H
#define SIPL_IO_PGMIO_H

#include "matrix/Matrix"
#include "io/IOBase.hpp"
#include "io/NetpbmIOBase.hpp"

namespace sipl
{
class PgmIO : public NetpbmIOBase
{
public:
    PgmIO() = default;

    // Reading
    static MatrixXb read(const char* filename);

    static MatrixXb read(const std::string& filename);

    // Writing
    static void write(const MatrixXb& mat,
                      const char* filename,
                      const FileType type = FileType::BINARY);

    static void write(const MatrixXb& mat,
                      const std::string& filename,
                      const FileType type = FileType::BINARY);

private:
    // look at magic number to determine file type
    static FileType determine_file_type(const std::string& filename);

    // Read a binary file
    static MatrixXb read_binary(const std::string& filename);

    // Read an ascii file
    static MatrixXb read_ascii(const std::string& filename);

    // Write binary file
    static void write_binary(const MatrixXb& mat, const std::string& filename);

    // Write binary file
    static void write_ascii(const MatrixXb& mat, const std::string& filename);
};
}

#endif
