/* Settings structure for BBM Fetch app and related functionality.

author: andreasl
*/
#pragma once

#include <string>

namespace barn {
namespace bbm {

/*Settings for the BBM Add app.*/
struct FetchSettings {
    std::string bookmarks_root_path;  /*root directory to the bookmarks*/
    std::string editor;  /*text editor to drop in for modification of bookmarks*/
};

/*Load FetchSettings from file.*/
void load_settings(const std::string& path, FetchSettings* settings);

} // namespace bbm
} // namespace barn
