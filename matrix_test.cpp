#include <iostream>
#include "matrix/Matrix"
#include "matrix/Vector"

using namespace sipl;

int main()
{
    Matrix33i m_static{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    MatrixXd m_dynamic{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    MatrixX<Vector3b> m_comp(4, 2);
    Vector3i v_static{1, 2, 3};
    std::cout << v_static / 3.0 << std::endl;
    std::cout << m_dynamic * m_static << std::endl;
}
