#include <iostream>
#include "matrix/Matrix"
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Matrix33i m_static{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    MatrixXd m_dynamic{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    MatrixX<Vector3b> m_comp(4, 2);
    auto m3 = m_static.apply([](auto e) { return double(e * 3 + 0.5); });
    std::cout << m3 << std::endl;
    std::cout << typeid(m3).name() << std::endl;
    std::cout << m_static.apply([](auto e) { return double(e * 3 + 0.5); })
              << std::endl;
}
