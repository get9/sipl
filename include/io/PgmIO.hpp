#pragma once

#ifndef SIPL_IO_PGMIO_H
#define SIPL_IO_PGMIO_H

#include <fstream>
#include <limits>
#include <string>
#include <stdexcept>
#include <cassert>
#include <sstream>
#include "matrix/Matrix.hpp"
#include "io/NetpbmIOBase.hpp"

namespace sipl
{
class PgmIO : public NetpbmIOBase
{
public:
    PgmIO() = default;

    // char* version
    Matrix<uint8_t, 2> read(const char* filename) const
    {
        return read(std::string(filename));
    }

    // std::string version
    Matrix<uint8_t, 2> read(const std::string& filename) const
    {
        PType type = determine_file_type(filename);
        switch (type) {
        case PType::BINARY:
            return read_binary<uint8_t>(filename);
        case PType::ASCII:
            return read_ascii<uint8_t>(filename);
        case PType::UNKNOWN:
            throw NetpbmIOException("Unknown file type, check magic number");
        }
    }

    // char* version
    void write(const Matrix<uint8_t, 2>& mat,
               const char* filename,
               const PType type = PType::BINARY) const
    {
        write(mat, std::string(filename), type);
    }

    // std::string version
    void write(const Matrix<uint8_t, 2>& mat,
               const std::string& filename,
               const PType type = PType::BINARY) const
    {
        switch (type) {
        case PType::ASCII:
            write_ascii<uint8_t>(mat, filename);
            break;
        case PType::BINARY:
            write_binary<uint8_t>(mat, filename);
            break;
        case PType::UNKNOWN:
            throw NetpbmIOException("Unknown file type");
        }
    }

private:
    // look at magic number to determine file type
    PType determine_file_type(const std::string& filename) const override;
};
}

#endif
