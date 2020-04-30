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
struct FetchSettings {
    fs::path bookmarks_root_path;  /*root directory to the bookmarks*/
    std::string editor;  /*text editor to drop in for modification of bookmarks*/
    std::string open_browser_command;  /*system command to open the browser*/
};

/*Load FetchSettings from file.*/
FetchSettings load_settings(const fs::path& path);

} // namespace bbm
} // namespace barn
