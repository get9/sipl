#include <iostream>
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Vector3i v{1, 2, 3};
    VectorXi v2{2, 4, 6};
    std::cout << "Static" << std::endl;
    std::cout << v << std::endl;
    auto new_v = v.apply_clone([](auto e) { return e << 1; });
    std::cout << new_v << std::endl;
    std::cout << v << std::endl;
    v += 2;
    std::cout << v << std::endl;

    // Add vectors of different types
    std::cout << v + v2 << std::endl;

    std::cout << "Dynamic" << std::endl;
    std::cout << v2 << std::endl;
    auto new_v2 = v.apply_clone([](auto e) { return e << 1; });
    std::cout << new_v2 << std::endl;
    std::cout << v2 << std::endl;
    v2 += 2;
    std::cout << v2 << std::endl;
}
