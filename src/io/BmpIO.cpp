#include <string>
#include <fstream>
#include <memory>
#include <limits>
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
    std::unique_ptr<char[]> buf(new char[size_t(file_length)]);
    stream.read(reinterpret_cast<char*>(buf.get()), file_length);

    // Fill BMPFILEHEADER and BMPINFOHEADER structs
    BITMAPFILEHEADER* file_header =
        reinterpret_cast<BITMAPFILEHEADER*>(buf.get());
    BITMAPINFOHEADER* info_header = reinterpret_cast<BITMAPINFOHEADER*>(
        buf.get() + sizeof(BITMAPFILEHEADER));

    // Fill the new matrix
    const auto data_offset = file_header->bfOffBits;
    MatrixX<uint8_t> img(info_header->biHeight, info_header->biWidth);
    const auto data_start = buf.get() + data_offset;

    // Calculate padded row size
    const int32_t padded_row_size =
        int32_t(std::floor((sizeof(uint8_t) * 8 * img.dims[1] + 31) / 32) * 4);

    // "normal" position, not in the "image" position
    for (int32_t i = img.dims[0] - 1; i >= 0; --i) {
        const auto row_start = data_start + i * padded_row_size;
        const auto row_end = row_start + info_header->biWidth;
        const auto mat_start =
            img.bytes() + (img.dims[0] - 1 - i) * img.dims[1];
        std::copy(row_start, row_end, mat_start);
    }

    return img;
}

void BmpIO::write(const MatrixX<uint8_t>& img, const char* filename)
{
    write(img, std::string(filename));
}

void BmpIO::write(const MatrixX<uint8_t>& img, const std::string& filename)
{
    std::ofstream stream{filename, std::ios::binary | std::ios::trunc};
    if (!stream) {
        throw IOException("Could not open file '" + filename +
                          "' for writing binary");
    }

    // Calculate padding if necessary
    // from here: https://en.wikipedia.org/wiki/BMP_file_format#Pixel_storage
    const int32_t padded_row_size =
        int32_t(std::floor((sizeof(uint8_t) * 8 * img.dims[1] + 31) / 32) * 4);

    // Color table. Needs to be for all gray values, last element is alpha (0)
    const int32_t color_table_size = (1 << 8) * 4;
    uint8_t color_table[color_table_size];
    for (int32_t i = 0; i < (1 << 8); ++i) {
        color_table[i * 4 + 0] = i;
        color_table[i * 4 + 1] = i;
        color_table[i * 4 + 2] = i;
        color_table[i * 4 + 3] = 0;
    }

    // Most of these values come from the MSDN documentation here:
    // https://msdn.microsoft.com/en-us/library/dd183376(v=vs.85).aspx
    // https://msdn.microsoft.com/en-us/library/dd183374(v=vs.85).aspx
    // Set up file header
    BITMAPFILEHEADER file_header;
    file_header.bfSize = padded_row_size * img.dims[0] + color_table_size +
                         sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    char type[] = {'B', 'M'};
    file_header.bfType = *reinterpret_cast<uint16_t*>(type);
    file_header.bfOffBits =
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + color_table_size;
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;

    // Set up info header
    BITMAPINFOHEADER info_header;
    info_header.biSize = 40;
    info_header.biWidth = img.dims[1];
    info_header.biHeight = img.dims[0];
    info_header.biPlanes = 1;
    info_header.biBitCount = 8;
    info_header.biCompression = 0;
    info_header.biSizeImage = img.dims[0] * img.dims[1];
    info_header.biXPelsPerMeter = 0;
    info_header.biYPelsPerMeter = 0;
    info_header.biClrUsed = std::numeric_limits<uint8_t>::max() + 1;
    info_header.biClrImportant = 0;

    // Start writing
    stream.write(reinterpret_cast<const char*>(&file_header),
                 sizeof(file_header));
    stream.write(reinterpret_cast<const char*>(&info_header),
                 sizeof(info_header));

    // Write color information. Note: this is not actually needed by images
    // written out, but it still has to be there according to spec
    stream.write(reinterpret_cast<const char*>(color_table), color_table_size);

    // Need to read in rows in reverse order because they put the origin in the
    // "normal" position, not in the "image" position
    for (int32_t i = img.dims[0] - 1; i >= 0; --i) {
        const auto row_start = i * img.dims[1];
        stream.write(img.as_bytes() + row_start, img.dims[1]);

        // Write padding, if necessary
        for (int32_t p = 0; p < (padded_row_size - img.dims[1]); ++p) {
            char zero = '\0';
            stream.write(&zero, 1);
        }
    }
}
