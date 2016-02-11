#include <iostream>
#include "io/PGMReader.hpp"
#include "matrix/Matrix.hpp"

using namespace sipl;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0] << " file.pgm" << std::endl;
        std::exit(1);
    }

    const std::string filename{argv[1]};
    PGMReader reader{filename};
    auto mat = reader.read8();
    std::cout << mat({0, 0}) << std::endl;
}
