#pragma once

#ifndef _SIPL_IO_PGMREADER_H_
#define _SIPL_IO_PGMREADER_H_

#include <string>
#include <fstream>
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

    PGMReader(const std::string& filename)
        : filename_(filename), image_width_(0), image_height_(0), maxval_(0)
    {
        type_ = determine_file_type();
        process_header();
    }

    template <typename TPixel>
    Matrix<TPixel> read(void) const
    {
        if (maxval_ > std::numeric_limits<uint8_t>::max()) {
            return read16();
        } else {
            return read8();
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
    FileType determine_file_type()
    {
        std::ifstream stream{filename_};
        if (!stream.is_open()) {
            throw IOException { "Could not open file to determine file type" }
        }

        const std::string type;
        stream >> type;
        if ("P5" == type) {
            return FileType::BINARY;
        } else if ("P2" == type) {
            return FileType::ASCII;
        } else {
            return FileType::UNKNOWN;
        }
    }

    // Process the header of both ASCII and Binary files
    void process_header(void)
    {
        std::ifstream stream{filename_};
        if (!stream.is_open()) {
            throw IOException{"Could not open file for reading header"};
        }

        // Check for comments, etc
        std::string word, comment_line;
        stream >> word;
        while ("#" == word.begin()) {
            std::getline(stream, comment_line);
            stream >> word;
        }

        width_ = size_t(std::stoi(word));

        stream >> word;
        while ("#" == word.begin()) {
            std::getline(stream, comment_line);
            stream >> word;
        }

        height_ = size_t(std::stoi(word));

        stream >> word;
        while ("#" == word.begin()) {
            std::getline(stream, comment_line);
            stream >> word;
        }

        maxval_ = std::stoi(word);
    }

    // Read 1-byte values
    Matrix<uint8_t> read8(void) const
    {
        switch (type_) {
            case FileType::BINARY: return read_binary<uint8_t>(); break;
            case FileType::ASCII: return read_ascii<uint8_t>(); break;
            case FileType::UNKNOWN:  // Fall through
            default: throw IOException{"Unknown PGM type"}; break;
        }
    }

    // Read 2-byte values
    Matrix<uint16_t> read16(void) const
    {
        switch (type_) {
            case FileType::BINARY: return read_binary<uint16_t>(); break;
            case FileType::ASCII: return read_ascii<uint16_t>(); break;
            case FileType::UNKNOWN:  // Fall through
            default: throw IOException{"Unknown PGM type"}; break;
        }
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
