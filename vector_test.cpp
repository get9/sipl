#include <iostream>
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Vector3i v{1, 2, 3};
    Vector3i v2{2, 4, 6};
    std::cout << v << std::endl;
    auto new_v = v.apply_clone([](auto e) { return e << 1; });
    std::cout << v << std::endl;
    std::cout << new_v << std::endl;
}
