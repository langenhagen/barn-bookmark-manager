/* Command line option-related functions.

author: andreasl
*/
#pragma once

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

/*Representation of the app's command line options.*/
struct Options {
    fs::path settings_path;  /*path to the settings file*/
};

/*Parse command line options.*/
Options parse_options(int argc, const char* argv[]);

} // namespace bbm
} // namespace barn
