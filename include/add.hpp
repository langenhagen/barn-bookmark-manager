/* Business logic for BBM Add app.

author: andreasl
*/
#pragma once

#include <experimental/filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

class AddSettings;
class Bookmark;

/*Fetch the url and the website tile from the focused Chrome top window.*/
bool fetch_url_and_title(std::string& url, std::string& title);

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const fs::path& directory);

} // namespace bbm
} // namespace barn
