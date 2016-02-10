#pragma once

#ifndef _SIPL_IO_PGMREADER_H_
#define _SIPL_IO_PGMREADER_H_

#include <fstream>
#include <limits>
#include <string>
#include <stdexcept>
#include "Matrix.hpp"

namespace sipl
{
class IOException : public std::exception
{
    explicit IOException(const char* msg) : msg_(msg) {}
    explicit IOException(const std::string& msg) : msg_(msg) {}
    virtual ~IOException() throw() {}
    virtual const char* what() const throw() { return msg_.c_str(); }
}

class PGMReader
{
public:
    // Different types of PGM files
    enum class FileType { BINARY, ASCII, UNKNOWN };

    PGMReader() = default;

    PGMReader(const std::string& filename);

    template <typename TPixel>
    Matrix<TPixel> read(void) const
    {
        if (maxval_ > std::numeric_limits<uint8_t>::max()) {
            return read_impl<uint8_t>();
        } else {
            return read_impl<uint16_t>();
        }
    }

private:
    std::string filename_;
    FileType type_;
    size_t image_width_;
    size_t image_height_;
    int32_t maxval_;

    // Figure out whether this is binary or ascii. Need to open file once to
    // look at magic number to determine file type
    FileType determine_file_type() const;

    // Process the header of both ASCII and Binary files
    void process_header(void);

    // Read 1-byte values
    Matrix<uint8_t> read8(void) const;

    // Read 2-byte values
    Matrix<uint16_t> read16(void) const;

    template <typename TPixel>
    Matrix<TPixel> read_impl(void) const
    {
        switch (type_) {
            case FileType::BINARY: return read_binary<TPixel>();
            case FileType::ASCII: return read_ascii<TPixel>();
            case FileType::UNKNOWN:  // Fall through
            default: throw IOException{"Unknown PGM type"};
        }
    }

    // Template specializations for the two types we know will be instantiated
    template <>
    read_impl<uint8_t>()
    {
    }
    template <>
    read_impl<uint16_t>()
    {
    }

    // Read a binary file
    template <typename TPixel>
    Matrix<PixelT> read_binary(void)
    {
    }

    // Read an ascii file
    template <typename TPixel>
    Matrix<PixelT> read_ascii(void)
    {
        // Check if we can open a file
        std::ifstream stream{filename_};
        if (!stream.is_open()) {
            throw IOException{"Could not open ASCII file for reading"};
        }

        // Fill header information
        process_header(stream);

        Matrix<TPixel> mat{image_height_, image_width_};
        for (size_t i = 0; i < image_height_ * image_width_; ++i) {
            stream >> mat[i];
        }

        return mat;
    }
};
}

#endif
