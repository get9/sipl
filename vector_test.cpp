#include <iostream>
#include "matrix/Vector"

using namespace sipl;

int main()
{
    VectorXi v(3);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    std::cout << v << std::endl;
}
