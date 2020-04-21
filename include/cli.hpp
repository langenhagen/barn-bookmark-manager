/*Command line option-related functions.

author: andreasl*/
#pragma once

#include <string>

namespace barn {
namespace bbm {

struct Options {
    std::string settings_path;  /*path to the settings file*/
};

/*Parse command line options.*/
Options parse_options(int argc, const char* argv[]);

} // namespace bbm
} // namespace barn
