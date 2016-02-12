#include <iostream>
#include "io/PgmIO.hpp"
#include "io/PpmIO.hpp"

using namespace sipl;

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
