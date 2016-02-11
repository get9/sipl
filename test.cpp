#include <iostream>
#include <algorithm>
#include "io/PGMIO.hpp"
#include "matrix/Matrix.hpp"

using namespace sipl;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0] << " file.pgm" << std::endl;
        std::exit(1);
    }

    // const std::string filename{argv[1]};
    PGMIO pgm;
    auto mat = pgm.read<uint8_t>("tmp_ascii.pgm");
    // pgm.write(mat, "tmp_binary.pgm", PGMIO::PType::BINARY);
    pgm.write(mat, "tmp_ascii2.pgm", PGMIO::PType::ASCII);
}
