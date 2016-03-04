#include <iostream>
#include "matrix/Matrix"
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Matrix33i m_static{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    MatrixXi m_dynamic{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    /*
    MatrixX<Vector3b> m_comp(2, 2, Vector3b{5, 5, 5});
    std::cout << "static: " << std::endl << m_static << std::endl;
    std::cout << "composite: " << std::endl << m_comp << std::endl;
    std::cout << "dynamic: " << std::endl << m_dynamic << std::endl;
    std::cout << std::endl;
    std::cout << "static * static: " << std::endl
              << m_static * m_static << std::endl;
    std::cout << "static * dynamic: " << std::endl
              << m_static * m_dynamic << std::endl;
    std::cout << "dynamic * static: " << std::endl
              << m_dynamic * m_static << std::endl;
    std::cout << "dynamic * dynamic: " << std::endl
              << m_dynamic * m_dynamic << std::endl;
    std::cout << "equality comparison: " << std::endl
              << (m_static * m_dynamic == m_dynamic * m_static) << std::endl;
    std::cout << "matrix * vector" << std::endl
              << m_static * Vector3i{1, 2, 3} << std::endl;
              */

    auto m2 = m_static * m_dynamic;
    std::cout << m2 << std::endl;
}
