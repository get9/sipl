#pragma once

#ifndef SIPL_IO_PPMIO_H
#define SIPL_IO_PPMIO_H

#include "cassert"
#include "io/NetpbmIOBase.hpp"
#include "matrix/Matrix.hpp"
#include "matrix/Vector.hpp"

namespace sipl
{
class PpmIO : public NetpbmIOBase
{
public:
    PpmIO() = default;

    // const char* version
    Matrix<RGBPixel> read(const char* filename) const
    {
        return read(std::string(filename));
    }

    // std::string version
    Matrix<RGBPixel> read(const std::string& filename) const
    {
        PType type = determine_file_type(filename);
        switch (type) {
        case PType::BINARY:
            return read_binary(filename);
        case PType::ASCII:
            return read_ascii(filename);
        case PType::UNKNOWN:
            throw NetpbmIOException("Unknown file type, check magic number");
        }
    }

    // char* version
    void write(const Matrix<RGBPixel>& mat,
               const char* filename,
               const PType type = PType::BINARY) const
    {
        write(mat, std::string(filename), type);
    }

    // std::string version
    void write(const Matrix<RGBPixel>& mat,
               const std::string& filename,
               const PType type = PType::BINARY) const
    {
        switch (type) {
        case PType::ASCII:
            write_ascii(mat, filename);
            break;
        case PType::BINARY:
            write_binary(mat, filename);
            break;
        case PType::UNKNOWN:
            throw NetpbmIOException("Unknown file type");
        }
    }

private:
    // look at magic number to determine file type
    PType determine_file_type(const std::string& filename) const;

    // Read a binary file
    Matrix<RGBPixel> read_binary(const std::string& filename) const
    {
        std::ifstream stream{filename, std::ios::binary};
        if (!stream) {
            throw NetpbmIOException("Could not open binary file for reading");
        }

        // Fill header information
        size_t height, width, maxval;
        std::tie(height, width, maxval) = process_header(stream);

        // Need to istream::get() because windows is dumb
        stream.get();

        // Assert check since I don't know how to do this with templates yet
        assert(maxval <= std::numeric_limits<uint8_t>::max() &&
               "wrong type for matrix");

        // Read binary data directly into the Matrix's data buffer
        Matrix<RGBPixel> mat{height, width};
        stream.read(mat.bytes(), ssize_t(mat.size_in_bytes()));
        return mat;
    }

    // Read an ascii file
    Matrix<RGBPixel> read_ascii(const std::string& filename) const
    {
        std::ifstream stream{filename, std::ios::binary};
        if (!stream) {
            throw NetpbmIOException("Could not open ascii file for reading");
        }

        // Fill header information
        size_t height, width, maxval;
        std::tie(height, width, maxval) = process_header(stream);

        Matrix<RGBPixel> mat{height, width};
        std::string r_str, g_str, b_str;
        for (size_t i = 0; i < mat.size(); ++i) {
            stream >> r_str;
            stream >> g_str;
            stream >> b_str;
            mat[i * 3 + 0] = uint8_t(std::stoul(r_str));
            mat[i * 3 + 1] = uint8_t(std::stoul(g_str));
            mat[i * 3 + 2] = uint8_t(std::stoul(b_str));
        }

        return mat;
    }

    // Write binary file
    void write_binary(const Matrix<RGBPixel>& mat,
                      const std::string& filename) const
    {
        std::ofstream stream{filename, std::ios::binary | std::ios::trunc};
        if (!stream) {
            throw NetpbmIOException("Could not open file for writing binary");
        }

        // Write magic number and matrix header info
        std::stringstream ss;
        ss << "P6" << std::endl
           << mat.dims[1] << " " << mat.dims[0] << std::endl
           << std::to_string(std::numeric_limits<uint8_t>::max()) << std::endl;
        stream.write(ss.str().c_str(), ssize_t(ss.str().size()));

        // Write mat data
        stream.write(mat.as_bytes(), ssize_t(mat.size_in_bytes()));
    }

    // Write binary file
    void write_ascii(const Matrix<RGBPixel>& mat,
                     const std::string& filename) const
    {
        std::ofstream stream{filename, std::ios::trunc | std::ios::binary};
        if (!stream) {
            throw NetpbmIOException("Could not open file for writing binary");
        }

        // Write magic number and matrix header info
        stream << "P3" << std::endl
               << mat.cols << " " << mat.rows << std::endl
               << std::to_string(std::numeric_limits<uint8_t>::max())
               << std::endl;

        // Write mat data
        for (size_t i = 0; i < mat.rows; ++i) {
            for (size_t j = 0; j < mat.cols; ++j) {
                auto channel = mat(i, j);
                stream << std::to_string(channel[0]) << " "
                       << std::to_string(channel[1]) << " "
                       << std::to_string(channel[2]) << " ";
            }
            stream << std::endl;
        }
    }
};
}

#endif
