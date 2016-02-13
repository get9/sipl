#include <iostream>
#include "io/PgmIO.hpp"
#include "io/PpmIO.hpp"
#include "improc/Improc.hpp"

using namespace sipl;

Matrix33d parse_transform(const std::string& filename);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0]
                  << " infile.pgm transform.txt outfile.pgm" << std::endl;
        std::exit(1);
    }
    // Matrix33d transform = {{0.707, 0.707, 0}, {-0.707, 0.707, 0}, {0, 0, 1}};
    Matrix33d transform = parse_transform(argv[2]);
    auto mat = PgmIO::read(argv[1]);
    auto new_mat = projective_transform<uint8_t, double>(
        mat, transform, InterpolateType::BILINEAR);
    PgmIO::write(new_mat, argv[3]);
}

Matrix33d parse_transform(const std::string& filename)
{
    std::ifstream stream{filename};
    if (!stream) {
        std::cerr << "Couldn't open transform file: " << filename << std::endl;
        std::exit(1);
    }

    Matrix33d m;
    std::string line;
    for (int32_t i = 0; i < 3; ++i) {
        std::getline(stream, line);
        std::stringstream ss{line};
        ss >> m(i, 0);
        ss >> m(i, 1);
        ss >> m(i, 2);
    }
    return m;
}

/*
int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0]
                  << " infile.pgm outfile.pgm [ascii/binary]" << std::endl;
        std::exit(1);
    }

    std::string write_type{argv[3]};
    std::string infile{argv[1]};
    std::string outfile{argv[2]};

    if (infile[infile.size() - 2] == 'g') {
        PgmIO reader;
        auto mat = reader.read(infile);
        if (write_type == "ascii") {
            PgmIO::write(mat, argv[2], PgmIO::FileType::ASCII);
        } else if (write_type == "binary") {
            PgmIO::write(mat, argv[2], PgmIO::FileType::BINARY);
        } else {
            std::cerr << "bad write type" << std::endl;
            std::exit(1);
        }
    } else {
        PpmIO reader;
        auto mat = reader.read(infile);
        if (write_type == "ascii") {
            reader.write(mat, argv[2], PpmIO::FileType::ASCII);
        } else if (write_type == "binary") {
            reader.write(mat, argv[2], PpmIO::FileType::BINARY);
        } else {
            std::cerr << "bad write type" << std::endl;
            std::exit(1);
        }
    }
}
*/
