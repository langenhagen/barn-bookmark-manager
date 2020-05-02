/* Implementation for log.hpp.

author: andreasl
*/
#include "log.hpp"

#include <iostream>

#define BBM_ENABLE_LOGGING

namespace barn {
namespace bbm {

namespace {

/*A dummy null output buffer for disabling logging.*/
struct NullBuffer : public std::streambuf {
    int overflow(int c) {
        return c;
    }
};
NullBuffer null_buffer;
std::ostream null_stream(&null_buffer);

} // namespace

std::ostream& log(const Severity level) {
#ifdef BBM_ENABLE_LOGGING
    if (level == Severity::INFO) {
        return std::cout;
    } else if (level == Severity::WARN) {
        return std::cerr << "Warning: ";
    } if (level == Severity::ERROR) {
        return std::cerr << "Error: ";
    }
#else
    return null_stream;
#endif
}

#undef BBM_ENABLE_LOGGING

} // namespace bbm
} // namespace barn
