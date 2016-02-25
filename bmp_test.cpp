#include <iostream>
#include "io/BmpIO.hpp"
#include "io/PgmIO.hpp"
#include "matrix/Matrix.hpp"

using namespace sipl;

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "    " << argv[0] << " infile.bmp outfile.pgm"
                  << std::endl;
        std::exit(1);
    }

    const std::string infile(argv[1]);
    const std::string outfile(argv[2]);

    const auto mat = BmpIO::read(infile);
    PgmIO::write(mat, outfile);
}
