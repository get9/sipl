#include <iostream>
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Vector3i v{1, 2, 3};
    VectorXi v2{2, 4, 6};
    std::cout << "static:  " << v << std::endl;
    std::cout << "dynamic: " << v2 << std::endl;
    std::cout << "apply_clone: ";
    std::cout << v.apply_clone([](auto e) { return e << 1; }) << std::endl;

    // Add vectors of different types
    std::cout << "Math operations" << std::endl;
    std::cout << v + v << std::endl;
    std::cout << v2 + v << std::endl;
    std::cout << v2 + v2 << std::endl;
    std::cout << v + v2 << std::endl;
    std::cout << std::endl;

    std::cout << "Comparison operations" << std::endl;
    std::cout << "operator==: " << (v == v2) << std::endl;
    std::cout << "operator<: " << (v < v2) << std::endl;
    std::cout << "operator>: " << (v > v2) << std::endl;
    std::cout << "operator<=: " << (v <= v2) << std::endl;
    std::cout << "operator>=: " << (v >= v2) << std::endl;
}
