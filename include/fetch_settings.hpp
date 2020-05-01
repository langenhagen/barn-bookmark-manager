/* Settings structure for BBM Fetch app and related functionality.

author: andreasl
*/
#pragma once

#include <experimental/filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

/*Settings for the BBM Add app.*/
struct Settings {
    fs::path bookmarks_root_path;  /*Root directory to the bookmarks.*/
    std::string editor;  /*Text editor to drop in for modification of bookmarks.*/
    std::string open_browser_command;  /*System command to open the browser.*/
};

/*Load Settings from file.*/
Settings load_settings(const fs::path& path);

} // namespace bbm
} // namespace barn
