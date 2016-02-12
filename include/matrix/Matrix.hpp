#pragma once

#ifndef SIPL_MATRIX_H
#define SIPL_MATRIX_H

#include <functional>
#include <numeric>
#include <array>
#include <memory>
#include <iostream>
#include "matrix/Vector.hpp"

namespace sipl
{
template <typename Dtype>
class Matrix
{
public:
    const std::array<size_t, 2> dims;
    const size_t rows;
    const size_t cols;

    // Default constructor, no data
    Matrix() : dims({0, 0}), rows(0), cols(0), nelements_(0), data_(nullptr) {}

    // Sized constructor. 2 stored in initializer list and initializes
    // nmemory
    Matrix(const size_t rs, const size_t cs)
        : dims({rs, cs})
        , rows(rs)
        , cols(cs)
        , nelements_(rows * cols)
        , nbytes_(nelements_ * sizeof(Dtype))
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_]))
    {
    }

    // Const accessor
    const Dtype& operator()(const size_t row, const size_t col) const
    {
        return data_[row * cols + col];
    }

    // Non-const accessor
    Dtype& operator()(const size_t row, const size_t col)
    {
        return data_[row * cols + col];
    }

    // Access elements by single index
    const Dtype& operator[](size_t index) const { return data_[index]; }

    Dtype& operator[](size_t index) { return data_[index]; }

    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.get());
    }

    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.get()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.get());
    }

    char* bytes(void) { return reinterpret_cast<char*>(data_.get()); }

    size_t size(void) const { return nelements_; }

    size_t size_in_bytes(void) const { return nbytes_; }

private:
    size_t nelements_;
    size_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};

// Partial specialization for Vector types
template <typename Dtype, size_t Length>
class Matrix<Vector<Dtype, Length>>
{
public:
    const std::array<size_t, 3> dims;
    const size_t rows;
    const size_t cols;

    // Default constructor, no data
    Matrix() : dims({0, 0, 0}), rows(0), cols(0), nelements_(0), data_(nullptr)
    {
    }

    // Sized constructor. 2 stored in initializer list and initializes
    // nmemory
    Matrix(const size_t rs, const size_t cs)
        : dims({rs, cs, Length})
        , rows(rs)
        , cols(cs)
        , nelements_(rows * cols)
        , nbytes_(nelements_ * sizeof(Dtype) * Length)
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_ * Length]))
    {
    }

    // Const accessor
    Vector<const Dtype, Length> operator()(const size_t row,
                                           const size_t col) const
    {
        return {data_[row * cols * Length + col * Length + 0],
                data_[row * cols * Length + col * Length + 1],
                data_[row * cols * Length + col * Length + 2]};
    }

    // Non-const accessor
    Vector<Dtype, Length> operator()(const size_t row, const size_t col)
    {
        return {data_[row * cols * Length + col * Length + 0],
                data_[row * cols * Length + col * Length + 1],
                data_[row * cols * Length + col * Length + 2]};
    }

    // Access elements by single index
    const Dtype& operator[](size_t i) const { return data_[i]; }

    Dtype& operator[](size_t i) { return data_[i]; }

    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.get());
    }

    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.get()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.get());
    }

    char* bytes(void) { return reinterpret_cast<char*>(data_.get()); }

    size_t size(void) const { return nelements_; }

    size_t size_in_bytes(void) const { return nbytes_; }

private:
    size_t nelements_;
    size_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};
}

#endif
