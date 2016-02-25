#pragma once

#ifndef NETPBM_IO_BASE_H
#define NETPBM_IO_BASE_H

#include <string>
#include <fstream>
#include <tuple>
#include <sstream>
#include "io/IOBase.hpp"
#include "matrix/Matrix.hpp"

namespace sipl
{

// From: http://stackoverflow.com/a/8152888
class NetpbmIOException : public std::exception
{
public:
    explicit NetpbmIOException(const char* msg) : msg_(msg) {}

    explicit NetpbmIOException(const std::string& msg) : msg_(msg) {}

    virtual const char* what() const noexcept { return msg_.c_str(); }

protected:
    std::string msg_;
};

class NetpbmIOBase : public IOBase
{
public:
    enum class FileType { BINARY, ASCII, UNKNOWN };

protected:
    static std::tuple<int32_t, int32_t, int32_t> process_header(
        std::ifstream& stream);
};
}

#endif
