/* Implementation for util.hpp.

author: andreasl
*/
#include "util.hpp"

#include <iostream>
#include <string>

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

/* Write a log message to console.*/
std::ostream& log(const Severity level) {
#ifdef BBM_ENABLE_LOGGING
    if(level == Severity::INFO) {
        return std::cout;
    } else if(level == Severity::WARN) {
        return std::cout << "Warning: ";
    } if(level == Severity::ERROR) {
        return std::cout << "Error: ";
    }
#else
    return null_stream;
#endif
}

#undef BBM_ENABLE_LOGGING

} // namespace bbm
} // namespace barn
