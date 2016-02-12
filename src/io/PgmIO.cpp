#include <iostream>
#include <tuple>
#include "io/PgmIO.hpp"

using namespace sipl;

// Figure out whether this is binary or ascii. Need to open file once to
// look at magic number to determine file type
PgmIO::PType PgmIO::determine_file_type(const std::string& filename) const
{
    std::ifstream stream{filename};
    if (!stream.is_open()) {
        throw NetpbmIOException{"Could not open file to determine file type"};
    }

    std::string type;
    stream >> type;
    if ("P5" == type) {
        return PType::BINARY;
    } else if ("P2" == type) {
        return PType::ASCII;
    } else {
        return PType::UNKNOWN;
    }
}
