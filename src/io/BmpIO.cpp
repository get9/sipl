#include <string>
#include <fstream>
#include "io/BmpIO.hpp"
#include "matrix/Matrix.hpp"

using namespace sipl;

MatrixX<uint8_t> BmpIO::read(const char* filename)
{
    return read(std::string(filename));
}

MatrixX<uint8_t> BmpIO::read(const std::string& filename)
{
    std::ifstream stream{filename, std::ios::binary};
    if (!stream) {
        throw IOException("Could not open file '" + filename + "'for reading");
    }

    // Get total file length
    stream.seekg(0, std::ios::end);
    std::streampos file_length = stream.tellg();
    stream.seekg(0, std::ios::beg);
    // std::cout << "file length: " << file_length << std::endl;

    // Read into buffer
    std::unique_ptr<char[]> buf(new char[file_length]);
    stream.read(reinterpret_cast<char*>(buf.get()), file_length);

    // Fill BMPFILEHEADER and BMPINFOHEADER structs
    BITMAPFILEHEADER* file_header =
        reinterpret_cast<BITMAPFILEHEADER*>(buf.get());
    BITMAPINFOHEADER* info_header = reinterpret_cast<BITMAPINFOHEADER*>(
        buf.get() + sizeof(BITMAPFILEHEADER));

    /*
    std::cout << "bfSize:        " << file_header->bfSize << std::endl;
    std::cout << "bfOffBits:     " << file_header->bfOffBits << std::endl;
    std::cout << "biSize:        " << info_header->biSize << std::endl;
    std::cout << "biWidth:       " << info_header->biWidth << std::endl;
    std::cout << "biHeight:      " << info_header->biHeight << std::endl;
    std::cout << "biBitCount:    " << info_header->biBitCount << std::endl;
    std::cout << "biCompression: " << info_header->biCompression << std::endl;
    std::cout << "biSizeImage:   " << info_header->biSizeImage << std::endl;
    */

    // Fill the new matrix
    // XXX There's a segfault on OSX somewhere in here, not sure where...
    // ASan nor Valgrind report anything useful on OSX
    const auto data_offset = file_header->bfOffBits;
    MatrixX<uint8_t> img(info_header->biHeight, info_header->biWidth);
    const auto data_start = buf.get() + data_offset;

    // Need to read in rows in reverse order because they put the origin in the
    // "normal" position, not in the "image" position
    for (int32_t i = img.dims[0] - 1; i >= 0; --i) {
        const auto row_start = data_start + i * img.dims[1];
        const auto row_end = row_start + info_header->biWidth;
        const auto mat_start =
            img.bytes() + (img.dims[0] - 1 - i) * img.dims[1];
        std::copy(row_start, row_end, mat_start);
    }

    return img;
}

/*
void write(const MatrixX<uint8_t>& mat, const char* filename)
{
    write(mat, std::string(filename));
}
*/

// void write(const MatrixX<uint8_t>& mat, const std::string& filename) {}
