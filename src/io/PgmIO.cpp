#include <iostream>
#include <tuple>
#include <cassert>
#include "io/PgmIO.hpp"

using namespace sipl;

MatrixX<uint8_t> PgmIO::read(const char* filename)
{
    return read(std::string(filename));
}

MatrixX<uint8_t> PgmIO::read(const std::string& filename)
{
    PgmIO::FileType type = determine_file_type(filename);
    switch (type) {
    case FileType::BINARY:
        return read_binary(filename);
    case FileType::ASCII:
        return read_ascii(filename);
    case FileType::UNKNOWN:
        throw NetpbmIOException("Unknown file type, check magic number");
    }
}

// char* version
void PgmIO::write(const MatrixX<uint8_t>& mat,
                  const char* filename,
                  const FileType type)
{
    write(mat, std::string(filename), type);
}

// std::string version
void PgmIO::write(const MatrixX<uint8_t>& mat,
                  const std::string& filename,
                  const FileType type)
{
    switch (type) {
    case FileType::ASCII:
        write_ascii(mat, filename);
        break;
    case FileType::BINARY:
        write_binary(mat, filename);
        break;
    case FileType::UNKNOWN:
        throw NetpbmIOException("Unknown file type");
    }
}

// Read a binary file
MatrixX<uint8_t> PgmIO::read_binary(const std::string& filename)
{
    std::ifstream stream{filename, std::ios::binary};
    if (!stream) {
        throw NetpbmIOException("Could not open binary file for reading");
    }

    // Fill header information
    size_t height, width, maxval;
    std::tie(height, width, maxval) = process_header(stream);

    // Assert check since I don't know how to do this with templates yet
    assert(maxval <= std::numeric_limits<uint8_t>::max() &&
           "wrong type for matrix");

    // Read binary data directly into the Matrix's data buffer
    MatrixX<uint8_t> mat{height, width};
    stream.read(mat.bytes(), ssize_t(mat.size_in_bytes()));
    return mat;
}

// Read an ascii file
MatrixX<uint8_t> PgmIO::read_ascii(const std::string& filename)
{
    std::ifstream stream{filename, std::ios::binary};
    if (!stream) {
        throw NetpbmIOException("Could not open ascii file for reading");
    }

    // Fill header information
    size_t height, width, maxval;
    std::tie(height, width, maxval) = process_header(stream);

    MatrixX<uint8_t> mat{height, width};
    std::string pixval;
    for (size_t i = 0; i < mat.size(); ++i) {
        stream >> pixval;
        mat[i] = uint8_t(std::stoul(pixval));
    }

    return mat;
}

// Write binary file
void PgmIO::write_binary(const MatrixX<uint8_t>& mat,
                         const std::string& filename)
{
    std::ofstream stream{filename, std::ios::binary | std::ios::trunc};
    if (!stream) {
        throw NetpbmIOException("Could not open file for writing binary");
    }

    // Write magic number and matrix header info
    std::stringstream ss;
    ss << "P5" << std::endl
       << mat.dims[1] << " " << mat.dims[0] << std::endl
       << std::to_string(std::numeric_limits<uint8_t>::max()) << std::endl;
    stream.write(ss.str().c_str(), ssize_t(ss.str().size()));

    // Write mat data
    stream.write(mat.as_bytes(), ssize_t(mat.size_in_bytes()));
}

// Write binary file
void PgmIO::write_ascii(const MatrixX<uint8_t>& mat,
                        const std::string& filename)
{
    std::ofstream stream{filename, std::ios::trunc | std::ios::binary};
    if (!stream) {
        throw NetpbmIOException("Could not open file for writing binary");
    }

    // Write magic number and matrix header info
    stream << "P2" << std::endl
           << mat.cols << " " << mat.rows << std::endl
           << std::to_string(std::numeric_limits<uint8_t>::max()) << std::endl;

    // Write mat data
    for (size_t i = 0; i < mat.rows; ++i) {
        for (size_t j = 0; j < mat.cols; ++j) {
            stream << std::to_string(mat(i, j)) << " ";
        }
        stream << std::endl;
    }
}

// Figure out whether this is binary or ascii. Need to open file once to
// look at magic number to determine file type
PgmIO::FileType PgmIO::determine_file_type(const std::string& filename)
{
    std::ifstream stream{filename};
    if (!stream.is_open()) {
        throw NetpbmIOException{"Could not open file to determine file type"};
    }

    std::string type;
    stream >> type;
    if ("P5" == type) {
        return FileType::BINARY;
    } else if ("P2" == type) {
        return FileType::ASCII;
    } else {
        return FileType::UNKNOWN;
    }
}
