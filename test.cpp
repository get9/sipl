#include <iostream>
#include <algorithm>
#include "io/PgmIO.hpp"
#include "matrix/Matrix.hpp"

using namespace sipl;

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0] << " infile.pgm outfile.pgm"
                  << std::endl;
        std::exit(1);
    }

    // const std::string filename{argv[1]};
    PgmIO pgm;
    auto mat = pgm.read(argv[1]);
    pgm.write(mat, argv[2], PGMIO::PType::BINARY);
    // pgm.write(mat, argv[2], PGMIO::PType::ASCII);
}
