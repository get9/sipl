#pragma once

#ifndef _SIPL_IO_PGMREADER_H_
#define _SIPL_IO_PGMREADER_H_

#include <fstream>
#include <limits>
#include <string>
#include <stdexcept>
#include <cassert>
#include "matrix/Matrix.hpp"

namespace sipl
{
// From: http://stackoverflow.com/a/8152888
class IOException : public std::exception
{
public:
    explicit IOException(const char* msg) : msg_(msg) {}
    explicit IOException(const std::string& msg) : msg_(msg) {}
    virtual ~IOException() throw() {}
    virtual const char* what() const throw() { return msg_.c_str(); }
protected:
    std::string msg_;
};

class PGMReader
{
public:
    // Different types of PGM files
    enum class PType { BINARY, ASCII, UNKNOWN };

    PGMReader() = default;

    PGMReader(const std::string& filename);

    Matrix<uint8_t> read8(void)
    {
        switch (type_) {
            case PType::BINARY: return read_binary<uint8_t>();
            case PType::ASCII: return read_ascii<uint8_t>();
            case PType::UNKNOWN:  // Fall through
            default: throw IOException{"Unknown PGM type"};
        }
    }

    Matrix<uint16_t> read16(void)
    {
        switch (type_) {
            case PType::BINARY: return read_binary<uint16_t>();
            case PType::ASCII: return read_ascii<uint16_t>();
            case PType::UNKNOWN:  // Fall through
            default: throw IOException{"Unknown PGM type"};
        }
    }

private:
    std::string filename_;
    PType type_;
    size_t image_width_;
    size_t image_height_;
    int32_t maxval_;

    // Figure out whether this is binary or ascii. Need to open file once to
    // look at magic number to determine file type
    PType determine_file_type() const;

    // Process the header of both ASCII and Binary files
    void process_header(std::ifstream& stream);

    // Read a binary file
    template <typename TPixel>
    Matrix<TPixel> read_binary(void)
    {
        std::ifstream stream{filename_};
        if (!stream.is_open()) {
            throw IOException("Could not open binary file for reading");
        }

        // Fill header information
        process_header(stream);

        // Read binary data directly into the Matrix's data buffer
        Matrix<TPixel> mat{image_height_, image_width_};
        stream.read((char*)(mat.data()), mat.size());
        return mat;
    }

    // Read an ascii file
    template <typename TPixel>
    Matrix<TPixel> read_ascii(void)
    {
        std::ifstream stream{filename_};
        if (!stream.is_open()) {
            throw IOException("Could not open ascii file for reading");
        }

        // Fill header information
        process_header(stream);

        Matrix<TPixel> mat{image_height_, image_width_};
        for (size_t i = 0; i < mat.size(); ++i) {
            stream >> mat[i];
        }

        return mat;
    }
};
}

#endif
