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

namespace sipl
{
// From: http://stackoverflow.com/a/8152888
class IOException : public std::exception
{
public:
    explicit IOException(const char* msg) : msg_(msg) {}

    explicit IOException(const std::string& msg) : msg_(msg) {}

    virtual const char* what() const noexcept { return msg_.c_str(); }

protected:
    std::string msg_;
};

class PGMIO
{
public:
    // Different types of PGM files
    enum class PType { BINARY, ASCII, UNKNOWN };

    PGMIO() = default;

    // char* version
    template <typename T>
    Matrix<T, 2> read(const char* filename) const
    {
        return read<T>(std::string(filename));
    }

    // std::string version
    template <typename T>
    Matrix<T, 2> read(const std::string& filename) const
    {
        PType type = determine_file_type(filename);
        switch (type) {
        case PType::BINARY:
            return read_binary<T>(filename);
        case PType::ASCII:
            return read_ascii<T>(filename);
        case PType::UNKNOWN:
            throw IOException("Unknown file type, check magic number");
        }
    }

    // char* version
    template <typename T>
    void write(const Matrix<T, 2>& mat,
               const char* filename,
               const PType type = PType::BINARY) const
    {
        write<T>(mat, std::string(filename), type);
    }

    // std::string version
    template <typename T>
    void write(const Matrix<T, 2>& mat,
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
            throw IOException("Unknown file type");
        }
    }

private:
    // look at magic number to determine file type
    PType determine_file_type(const std::string& filename) const;

    // Process the header of both ASCII and Binary files
    std::tuple<size_t, size_t, size_t> process_header(
        std::ifstream& stream) const;

    // Read a binary file
    template <typename T>
    Matrix<T, 2> read_binary(const std::string& filename) const
    {
        std::ifstream stream{filename, std::ios::binary};
        if (!stream) {
            throw IOException("Could not open binary file for reading");
        }

        // Fill header information
        size_t height, width, maxval;
        std::tie(height, width, maxval) = process_header(stream);

        // Assert check since I don't know how to do this with templates yet
        assert(maxval <= std::numeric_limits<T>::max() &&
               "wrong type for matrix");

        // Read binary data directly into the Matrix's data buffer
        Matrix<T, 2> mat({height, width});
        stream.read(mat.bytes(), ssize_t(mat.size_in_bytes()));
        return mat;
    }

    // Read an ascii file
    template <typename T>
    Matrix<T, 2> read_ascii(const std::string& filename) const
    {
        std::ifstream stream{filename, std::ios::binary};
        if (!stream) {
            throw IOException("Could not open ascii file for reading");
        }

        // Fill header information
        size_t height, width, maxval;
        std::tie(height, width, maxval) = process_header(stream);

        Matrix<T, 2> mat({height, width});
        std::string pixval;
        for (size_t i = 0; i < mat.size(); ++i) {
            stream >> pixval;
            mat[i] = T(std::stoul(pixval));
        }

        return mat;
    }

    // Write binary file
    template <typename T>
    void write_binary(const Matrix<T, 2>& mat,
                      const std::string& filename) const
    {
        std::ofstream stream{filename, std::ios::binary | std::ios::trunc};
        if (!stream) {
            throw IOException("Could not open file for writing binary");
        }

        // Write magic number and matrix header info
        std::stringstream ss;
        ss << "P5" << std::endl
           << mat.dims[1] << " " << mat.dims[0] << std::endl
           << std::to_string(std::numeric_limits<T>::max()) << std::endl;
        stream.write(ss.str().c_str(), ssize_t(ss.str().size()));

        // Write mat data
        stream.write(mat.as_bytes(), ssize_t(mat.size_in_bytes()));
    }

    // Write binary file
    template <typename T>
    void write_ascii(const Matrix<T, 2>& mat, const std::string& filename) const
    {
        std::ofstream stream{filename, std::ios::trunc | std::ios::binary};
        if (!stream) {
            throw IOException("Could not open file for writing binary");
        }

        // Write magic number and matrix header info
        stream << "P2" << std::endl
               << mat.dims[1] << " " << mat.dims[0] << std::endl
               << std::to_string(std::numeric_limits<T>::max()) << std::endl;

        // Write mat data
        for (size_t i = 0; i < mat.dims[0]; ++i) {
            for (size_t j = 0; j < mat.dims[1]; ++j) {
                stream << std::to_string(mat({i, j})) << " ";
            }
            stream << std::endl;
        }
    }
};
}

#endif