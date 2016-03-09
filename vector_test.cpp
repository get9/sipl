#include <iostream>
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Vector3i v1{1, 2, 3};
    VectorXi v2{2, 4, 6};
    auto v3 = v2.apply([](auto e) { return double(e * 2); });
    std::cout << typeid(v1).name() << std::endl;
    std::cout << typeid(v2).name() << std::endl;
    std::cout << typeid(v3).name() << std::endl;
    /*
    std::cout << "static:  " << v << std::endl;
    std::cout << "dynamic: " << v2 << std::endl;

    // Add vectors of different types
    std::cout << "Math operations" << std::endl;
    std::cout << v + v << std::endl;
    std::cout << v2 + v << std::endl;
    std::cout << v2 + v2 << std::endl;
    std::cout << v + v2 << std::endl;
    std::cout << std::endl;
    std::cout << "dynamic - scalar: " << v2 - 1 << std::endl;
    std::cout << "static - scalar: " << v - 1 << std::endl;
    std::cout << "static: " << v << std::endl;

    std::cout << "Comparison operations" << std::endl;
    std::cout << "operator==: " << (v == v2) << std::endl;
    std::cout << "operator<: " << (v < v2) << std::endl;
    std::cout << "operator>: " << (v > v2) << std::endl;
    std::cout << "operator<=: " << (v <= v2) << std::endl;
    std::cout << "operator>=: " << (v >= v2) << std::endl;
    */
}
