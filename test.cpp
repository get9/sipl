#include <iostream>
#include "PGMReader.hpp"
#include "matrix/Matrix.hpp"

int main(int argc, char** argv)
{
    const std::string filename = "test.pgm";
    PGMReader reader{filename};
    auto mat = reader.read();
    std::cout << mat({0, 0}) << std::endl;
}
