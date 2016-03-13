#pragma once

#ifndef SIPL_IMPROC_KERNELS_H
#define SIPL_IMPROC_KERNELS_H

#include "matrix/Matrix"

namespace sipl
{

// clang-format off
namespace Kernels {
    static const Matrix33d SobelX{{-1, 0, 1},
                                  {-2, 0, 2},
                                  {-1, 0, 1}};

    static const Matrix33d SobelY{{-1, -2, -1},
                                  { 0,  0,  0},
                                  { 1,  2,  1}};

    static const Matrix33d PrewittX{{-1, 0, 1},
                                    {-1, 0, 1},
                                    {-1, 0, 1}};

    static const Matrix33d PrewittY{{-1, -1, -1},
                                    { 0,  0,  0},
                                    { 1,  1,  1}};

    static const Matrix33d ScharrX{{ -3, 0,  3},
                                   {-10, 0, 10},
                                   { -3, 0,  3}};

    static const Matrix33d ScharrY{{-3, -10, -3},
                                   { 0,   0,  0},
                                   { 3,  10,  3}};

}
// clang-format on
}

#endif
