/* Common settings-related functionality.

author: andreasl
*/
#pragma once

#include <string>

namespace barn {
namespace bbm {

/*Write the default settings to the given file paths.*/
bool write_default_settings(const std::string& path);

} // namespace bbm
} // namespace barn
