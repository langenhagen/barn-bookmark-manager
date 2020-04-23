/* Common utility functions.

author: andreasl
*/
#pragma once

#include <ostream>

namespace barn {
namespace bbm {

/* Severity levels for logging.*/
enum Severity {
    INFO,
    WARN,
    ERROR
};

/* Write a log message.*/
std::ostream& log(const Severity level);

} // namespace bbm
} // namespace barn
