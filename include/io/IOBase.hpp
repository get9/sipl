#pragma once

#ifndef SIPL_IO_IOBASE_HPP
#define SIPL_IO_IOBASE_HPP

namespace sipl
{

// From: http://stackoverflow.com/a/8152888
class IOException : public std::exception
{
public:
    explicit IOException(const char* msg) : msg_(msg) {}

    explicit IOException(const std::string& msg) : msg_(msg) {}

    virtual const char* what() const noexcept { return msg_.c_str(); }

protected:
    std::string msg_;
};

class IOBase
{
};
}

#endif
