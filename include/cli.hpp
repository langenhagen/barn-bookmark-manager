/* Command line option-related functions.

author: andreasl
*/
#pragma once

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

namespace exitcode {

constexpr const int SUCCESS = 0;
constexpr const int WRONG_CLI_ARGUMENTS = 1;
constexpr const int SYSTEM_ERROR = 2;
constexpr const int WRONG_INPUT = 4;

} // namespace exitcode

/*Representation of the app's command line options.*/
struct Options {
    fs::path settings_path;  /*path to the settings file*/
};

/*Parse command line options.*/
Options parse_options(int argc, const char* argv[]);

} // namespace bbm
} // namespace barn
