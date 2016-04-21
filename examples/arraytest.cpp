#include <iostream>
#include "ndarray/ndarray.hpp"

int main()
{
    size_t dim0, dim1, dim2;
    std::cin >> dim0 >> dim1 >> dim2;
    sipl::array<int32_t> a(dim0, dim1, dim2);

    sipl::array<int32_t> b;
    std::cout << a.size() << std::endl;
    return 0;
}