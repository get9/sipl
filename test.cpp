#include <iostream>
#include <algorithm>
#include "matrix/Matrix.hpp"
#include "io/PpmIO.hpp"
#include "io/PgmIO.hpp"

using namespace sipl;

int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0]
                  << " infile.pgm outfile.pgm [ascii/binary]" << std::endl;
        std::exit(1);
    }

    PgmIO ppm;
    std::string write_type{argv[3]};
    auto mat = ppm.read(argv[1]);
    if (write_type == "ascii") {
        ppm.write(mat, argv[2], PpmIO::PType::ASCII);
    } else if (write_type == "binary") {
        ppm.write(mat, argv[2], PgmIO::PType::BINARY);
    } else {
        std::cerr << "bad write type" << std::endl;
        std::exit(1);
    }
}
