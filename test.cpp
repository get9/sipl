#include <iostream>
#include <algorithm>
#include "matrix/Matrix.hpp"
#include "io/PpmIO.hpp"

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
    PpmIO ppm;
    auto mat = ppm.read(argv[1]);
    ppm.write(mat, argv[2], PpmIO::PType::BINARY);
    // ppm.write(mat, argv[2], PpmIO::PType::ASCII);
}
