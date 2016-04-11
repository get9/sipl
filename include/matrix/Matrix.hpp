#pragma once

#ifndef SIPL_MATRIX_MATRIX_H
#define SIPL_MATRIX_MATRIX_H

#include "matrix/MatrixBase.hpp"
#include "matrix/Vector.hpp"
#include "matrix/Wrappers.hpp"
#include <cmath>
#include <iostream>

namespace sipl
{

// Need to use 'this' pointer below because templated base class members are
// not visible in a certain phase of compilation. See here:
// http://stackoverflow.com/a/6592617

// Initial class for statically-allocated matrices
template <typename Dtype, int32_t Rows, int32_t Cols>
class Matrix : public MatrixBase<Dtype,
                                 Rows,
                                 Cols,
                                 StaticArrayWrapper<Dtype, Rows * Cols>>
{
public:
    using ContainerType = StaticArrayWrapper<Dtype, Rows * Cols>;
    using BaseClass = MatrixBase<Dtype, Rows, Cols, ContainerType>;
    using BaseClass::BaseClass;

    // Explicitly import BaseClass member variables so we don't have to use
    // 'this' pointer because of 2-phase lookup (see here for more information:
    // http://stackoverflow.com/a/4643295)
    using BaseClass::nelements_;
    using BaseClass::nbytes_;
    using BaseClass::data_;
    using BaseClass::dims;

    Matrix() : BaseClass() {}

    Matrix(Dtype fill_value) : BaseClass()
    {
        std::fill(std::begin(data_), std::end(data_), fill_value);
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
        : BaseClass()
    {
        assert(int32_t(list.size()) == Rows && "size mismatch");
        for (const auto l : list) {
            assert(int32_t(l.size()) == Cols && "size mismatch");
        }

        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l), std::end(l),
                      std::begin(data_) + i++ * l.size());
        }
    }

    template <typename OtherType>
    Matrix<OtherType, Rows, Cols> as_type() const
    {
        return apply([](auto e) { return OtherType(e); });
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Matrix<OutputType, Rows, Cols> new_m;
        std::transform(std::begin(*this), std::end(*this), std::begin(new_m),
                       f);
        return new_m;
    }

    Matrix rescale(Dtype new_min, Dtype new_max) const
    {
        return apply([ min = this->min(), max = this->max(), new_min, new_max ](
            auto e) {
            return Dtype(((new_max - new_min) / double(max - min)) * e +
                         ((new_min * max + min * new_max) / double(max - min)));
        });
    }

    Matrix clip(Dtype new_min, Dtype new_max) const
    {
        Matrix new_m;
        for (int32_t i = 0; i < this->size(); ++i) {
            auto e = std::round((*this)[i]);
            if (e < new_min) {
                new_m[i] = new_min;
            } else if (e > new_max) {
                new_m[i] = new_max;
            } else {
                new_m[i] = e;
            }
        }

        return new_m;
    }
};

// Specialization of the above for dynamically-allocated matrices.
template <typename Dtype>
class Matrix<Dtype, Dynamic, Dynamic>
    : public MatrixBase<Dtype,
                        Dynamic,
                        Dynamic,
                        DynamicArrayWrapper<Dtype, Dynamic>>
{
public:
    using ContainerType = DynamicArrayWrapper<Dtype, Dynamic>;
    using BaseClass = MatrixBase<Dtype, Dynamic, Dynamic, ContainerType>;
    using BaseClass::BaseClass;

    using BaseClass::nelements_;
    using BaseClass::nbytes_;
    using BaseClass::data_;
    using BaseClass::dims;

    Matrix(int32_t rows, int32_t cols)
    {
        nelements_ = rows * cols;
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, Dtype fill_value)
    {
        nelements_ = rows * cols;
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        dims = {rows, cols};
        std::fill(std::begin(data_), std::end(data_), fill_value);
    }

    Matrix(std::array<int32_t, 2> new_dims, Dtype fill_value)
    {
        nelements_ = new_dims[0] * new_dims[1];
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        dims = new_dims;
        std::fill(std::begin(data_), std::end(data_), fill_value);
    }

    Matrix(std::array<int32_t, 2> new_dims)
    {
        nelements_ = new_dims[0] * new_dims[1];
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        dims = new_dims;
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        int32_t nrows = list.size();
        int32_t ncols = list.begin()->size();
        for (const auto l : list) {
            assert(int32_t(l.size()) == ncols &&
                   "initializer_list size mismatch");
        }

        nelements_ = nrows * ncols;
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        data_ = ContainerType(nelements_);
        dims = {nrows, ncols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l), std::end(l),
                      std::begin(data_) + i++ * l.size());
        }
    }

    // Extract a patch centered at (center_y, center_x) with radius ry and
    // rx. Change boundaries depending on BorderType
    Matrix patch(int32_t center_y,
                 int32_t center_x,
                 int32_t ry,
                 int32_t rx,
                 const BorderType border_type = BorderType::REPLICATE) const
    {
        assert(center_y >= 0 && center_y < dims[0] && "center_y out of bounds");
        assert(center_x >= 0 && center_x < dims[1] && "center_x out of bounds");

        Matrix patch(2 * ry + 1, 2 * rx + 1);
        for (int32_t y = center_y - ry, r = 0; y <= center_y + ry; ++y, ++r) {
            for (int32_t x = center_x - rx, c = 0; x <= center_x + rx;
                 ++x, ++c) {
                switch (border_type) {
                case BorderType::REPLICATE:
                    patch(r, c) = (*this)(this->clamp_row_index(y),
                                          this->clamp_col_index(x));
                }
            }
        }
        return patch;
    }

    template <typename OtherType>
    Matrix<OtherType, Dynamic, Dynamic> as_type() const
    {
        return apply([](auto e) { return OtherType(e); });
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Matrix<OutputType, Dynamic, Dynamic> new_m(dims);
        std::transform(std::begin(*this), std::end(*this), std::begin(new_m),
                       f);
        return new_m;
    }

    Matrix clip(Dtype new_min, Dtype new_max) const
    {
        Matrix new_m(dims);
        for (int32_t i = 0; i < this->size(); ++i) {
            auto e = std::round((*this)[i]);
            if (e < new_min) {
                new_m[i] = new_min;
            } else if (e > new_max) {
                new_m[i] = new_max;
            } else {
                new_m[i] = e;
            }
        }

        return new_m;
    }

    Matrix rescale(Dtype new_min, Dtype new_max) const
    {
        return apply([ min = this->min(), max = this->max(), new_min, new_max ](
            auto e) {
            return Dtype(((new_max - new_min) / double(max - min)) * e +
                         ((new_min * max + min * new_max) / double(max - min)));
        });
    }
};

// Specialization of the above for dynamically-allocated Matrix with Vector
// elements.
// XXX Only specialized for fixed-sized Vector elements at this time
template <typename Dtype, int32_t Length>
class Matrix<Vector<Dtype, Length>, Dynamic, Dynamic>
    : public MatrixBase<Vector<Dtype, Length>,
                        Dynamic,
                        Dynamic,
                        DynamicArrayWrapper<Vector<Dtype, Length>, Length>>
{
public:
    using value_type = Vector<Dtype, Length>;
    using ContainerType = DynamicArrayWrapper<value_type, Length>;
    using BaseClass = MatrixBase<value_type, Dynamic, Dynamic, ContainerType>;
    using BaseClass::BaseClass;

    using BaseClass::nelements_;
    using BaseClass::nbytes_;
    using BaseClass::data_;
    using BaseClass::dims;

    Matrix(std::array<int32_t, 2> dims_)
    {
        nelements_ = dims_[0] * dims_[1];
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        dims = dims_;
    }

    Matrix(std::array<int32_t, 2> dims_, Dtype fillval)
    {
        nelements_ = dims_[0] * dims_[1];
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        dims = dims_;
        std::fill(std::begin(data_), std::end(data_), fillval);
    }

    Matrix(int32_t rows, int32_t cols)
    {
        nelements_ = rows * cols;
        nbytes_ = nelements_ * Length * int32_t(sizeof(Dtype));
        data_ = ContainerType(nelements_);
        dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, value_type fill_value)
    {
        nelements_ = rows * cols;
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * Length * int32_t(sizeof(Dtype));
        dims = {rows, cols};
        std::fill(std::begin(data_), std::end(data_), fill_value);
    }

    // Extract a patch centered at (center_y, center_x) with radius ry and
    // rx. Change boundaries depending on BorderType
    Matrix patch(int32_t center_y,
                 int32_t center_x,
                 int32_t ry,
                 int32_t rx,
                 const BorderType border_type = BorderType::REPLICATE) const
    {
        assert(center_y >= 0 && center_y < dims[0] && "center_y out of bounds");
        assert(center_x >= 0 && center_x < dims[1] && "center_x out of bounds");

        Matrix patch(2 * ry + 1, 2 * rx + 1);
        for (int32_t y = center_y - ry, r = 0; y <= center_y + ry; ++y, ++r) {
            for (int32_t x = center_x - rx, c = 0; x <= center_x + rx;
                 ++x, ++c) {
                switch (border_type) {
                case BorderType::REPLICATE:
                    patch(r, c) = (*this)(this->clamp_row_index(y),
                                          this->clamp_col_index(x));
                }
            }
        }
        return patch;
    }

    template <typename OtherType>
    Matrix<OtherType, Dynamic, Dynamic> as_type() const
    {
        return apply([](auto e) { return OtherType(e); });
    }

    Matrix clip(Dtype new_min, Dtype new_max) const
    {
        Matrix new_m(dims);
        for (int32_t i = 0; i < this->size(); ++i) {
            for (int32_t c = 0; c < new_m[0].size(); ++c) {
                auto e = std::round((*this)[i][c]);
                if (e < new_min) {
                    new_m[i][c] = new_min;
                } else if (e > new_max) {
                    new_m[i][c] = new_max;
                } else {
                    new_m[i][c] = e;
                }
            }
        }

        return new_m;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss << "[";
        for (int32_t i = 0; i < dims[0] - 1; ++i) {
            for (int32_t j = 0; j < dims[1] - 1; ++j) {
                ss << (*this)(i, j) << " ";
            }
            ss << (*this)(i, dims[1] - 1) << ";" << std::endl;
        }
        for (int32_t j = 0; j < dims[1] - 1; ++j) {
            ss << (*this)(dims[0] - 1, j) << " ";
        }
        ss << this->back() << "]";
        return ss.str();
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType = typename std::result_of<
                  UnaryFunctor&(Vector<Dtype, Length>)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Matrix<OutputType, Dynamic, Dynamic> new_m(dims);
        std::transform(std::begin(*this), std::end(*this), std::begin(new_m),
                       f);
        return new_m;
    }
};

// Static aliases
template <typename T>
using Matrix44 = Matrix<T, 4, 4>;
using Matrix44d = Matrix44<double>;
template <typename T>
using Matrix33 = Matrix<T, 3, 3>;
using Matrix33i = Matrix33<int32_t>;
using Matrix33d = Matrix33<double>;
using Matrix33f = Matrix33<float>;
using Matrix33b = Matrix33<uint8_t>;

// Dynamic aliases
template <typename T>
using MatrixX = Matrix<T, Dynamic, Dynamic>;
using MatrixXd = MatrixX<double>;
using MatrixXi = MatrixX<int32_t>;
using MatrixXb = MatrixX<uint8_t>;
}

#endif
