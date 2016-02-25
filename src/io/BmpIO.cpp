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
    std::cout << "file length: " << file_length << std::endl;

    // Read into buffer
    std::unique_ptr<char[]> buf(new char[file_length]);
    stream.read(reinterpret_cast<char*>(buf.get()), file_length);

    // Fill BMPFILEHEADER and BMPINFOHEADER structs
    BITMAPFILEHEADER* file_header =
        reinterpret_cast<BITMAPFILEHEADER*>(buf.get());
    BITMAPINFOHEADER* info_header = reinterpret_cast<BITMAPINFOHEADER*>(
        buf.get() + sizeof(BITMAPFILEHEADER));

    std::cout << "bfSize:        " << file_header->bfSize << std::endl;
    std::cout << "bfOffBits:     " << file_header->bfOffBits << std::endl;
    std::cout << "biSize:        " << info_header->biSize << std::endl;
    std::cout << "biWidth:       " << info_header->biWidth << std::endl;
    std::cout << "biHeight:      " << info_header->biHeight << std::endl;
    std::cout << "biBitCount:    " << info_header->biBitCount << std::endl;
    std::cout << "biCompression: " << info_header->biCompression << std::endl;
    std::cout << "biSizeImage:   " << info_header->biSizeImage << std::endl;

    // Fill the new matrix
    const auto data_offset = file_header->bfOffBits;
    MatrixX<uint8_t> img(info_header->biHeight, info_header->biWidth);
    const auto data_start = buf.get() + data_offset;
    const auto data_end = data_start + info_header->biSizeImage;
    std::cout << "data start: " << std::to_string(data_start - buf.get())
              << std::endl;
    std::cout << "data end:   " << std::to_string(data_end - buf.get())
              << std::endl;
    std::copy(data_start, data_end, img.bytes());
    std::cout << buf.get()[10000] << std::endl;
    return img;
}

/*
void write(const MatrixX<uint8_t>& mat, const char* filename)
{
    write(mat, std::string(filename));
}
*/

// void write(const MatrixX<uint8_t>& mat, const std::string& filename) {}
