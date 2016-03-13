#pragma once

#ifndef SIPL_MATRIX_MATRIX_H
#define SIPL_MATRIX_MATRIX_H

#include <iostream>
#include "matrix/Wrappers.hpp"
#include "matrix/Vector.hpp"
#include "matrix/MatrixBase.hpp"

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

    Matrix()
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        this->dims = {Rows, Cols};
    }

    Matrix(Dtype fill_value)
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        this->dims = {Rows, Cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        assert(int32_t(list.size()) == Rows && "size mismatch");
        for (const auto l : list) {
            assert(int32_t(l.size()) == Cols && "size mismatch");
        }

        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        this->dims = {Rows, Cols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l), std::end(l),
                      std::begin(this->data_) + i++ * l.size());
        }
    }

    // Convert one matrix type to another
    template <typename OtherType>
    Matrix(const Matrix<OtherType, Rows, Cols>& other)
    {
        this->dims = other.dims;
        this->nelements_ = other.size();
        this->nbytes_ = other.size_in_bytes();
        this->data_ = ContainerType();
        std::transform(std::begin(other), std::end(other),
                       std::begin(this->data_),
                       [](auto e) { return Dtype(e); });
    }

    template <typename OtherType>
    Matrix<OtherType, Rows, Cols> as_type() const
    {
        const auto max = std::numeric_limits<OtherType>::max();
        const auto tmp = (*this) / double(this->max());
        return tmp.apply(
            [max](auto e) { return OtherType(std::round(e * max)); });
    }

    template <typename UnaryFunctor>
    void transform(UnaryFunctor f)
    {
        std::transform(this->begin(), this->end(), this->begin(), f);
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Matrix<OutputType, Rows, Cols> new_m;
        std::transform(this->begin(), this->end(), std::begin(new_m), f);
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

    Matrix(int32_t rows, int32_t cols)
    {
        this->nelements_ = rows * cols;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, Dtype fill_value)
    {
        this->nelements_ = rows * cols;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = {rows, cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Matrix(std::array<int32_t, 2> new_dims)
    {
        this->nelements_ = new_dims[0] * new_dims[1];
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = new_dims;
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        int32_t nrows = list.size();
        int32_t ncols = list.begin()->size();
        for (const auto l : list) {
            assert(int32_t(l.size()) == ncols &&
                   "initializer_list size mismatch");
        }

        this->nelements_ = nrows * ncols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType(this->nelements_);
        this->dims = {nrows, ncols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l), std::end(l),
                      std::begin(this->data_) + i++ * l.size());
        }
    }

    // Convert one matrix type to another
    template <typename OtherType>
    Matrix(const Matrix<OtherType, Dynamic, Dynamic>& other)
    {
        this->dims = other.dims;
        this->nelements_ = other.size();
        this->nbytes_ = other.size_in_bytes();
        this->data_ = ContainerType(this->nelements_);
        std::transform(std::begin(other), std::end(other),
                       std::begin(this->data_),
                       [](auto e) { return Dtype(e); });
    }

    // Extract a patch centered at (center_y, center_x) with radius ry and
    // rx. Change boundaries depending on BorderType
    Matrix patch(int32_t center_y,
                 int32_t center_x,
                 int32_t ry,
                 int32_t rx,
                 const BorderType border_type = BorderType::REPLICATE) const
    {
        assert(center_y >= 0 && center_y < this->dims[0] &&
               "center_y out of bounds");
        assert(center_x >= 0 && center_x < this->dims[1] &&
               "center_x out of bounds");

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
        const auto max = std::numeric_limits<OtherType>::max();
        const auto tmp = (*this) / double(this->max());
        return tmp.apply(
            [max](auto e) { return OtherType(std::round(e * max)); });
    }

    template <typename UnaryFunctor>
    void transform(UnaryFunctor f)
    {
        std::transform(this->begin(), this->end(), this->begin(), f);
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Matrix<OutputType, Dynamic, Dynamic> new_m(this->dims);
        std::transform(this->begin(), this->end(), std::begin(new_m), f);
        return new_m;
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

    Matrix(std::array<int32_t, 2> dims)
    {
        this->nelements_ = dims[0] * dims[1];
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = dims;
    }

    Matrix(int32_t rows, int32_t cols)
    {
        this->nelements_ = rows * cols;
        this->nbytes_ = this->nelements_ * Length * int32_t(sizeof(Dtype));
        this->data_ = ContainerType(this->nelements_);
        this->dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, value_type fill_value)
    {
        this->nelements_ = rows * cols;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * Length * int32_t(sizeof(Dtype));
        this->dims = {rows, cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    // Convert one matrix type to another
    template <typename OtherType>
    Matrix(const Matrix<Vector<OtherType, Length>, Dynamic, Dynamic>& other)
    {
        this->dims = other.dims;
        this->nelements_ = other.size();
        this->nbytes_ = other.size_in_bytes();
        this->data_ = ContainerType(this->nelements_);
        std::transform(std::begin(other), std::end(other),
                       std::begin(this->data_),
                       [](auto e) { return value_type(e); });
    }

    // Extract a patch centered at (center_y, center_x) with radius ry and
    // rx. Change boundaries depending on BorderType
    Matrix patch(int32_t center_y,
                 int32_t center_x,
                 int32_t ry,
                 int32_t rx,
                 const BorderType border_type = BorderType::REPLICATE) const
    {
        assert(center_y >= 0 && center_y < this->dims[0] &&
               "center_y out of bounds");
        assert(center_x >= 0 && center_x < this->dims[1] &&
               "center_x out of bounds");

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

    template <typename UnaryFunctor>
    void transform(UnaryFunctor f)
    {
        std::transform(this->begin(), this->end(), this->begin(), f);
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Matrix<OutputType, Dynamic, Dynamic> new_m(this->dims);
        std::transform(this->begin(), this->end(), std::begin(new_m), f);
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
