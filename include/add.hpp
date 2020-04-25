/* Business logic for BBM Add app.

author: andreasl
*/
#pragma once

#include <experimental/filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

class Bookmark;

/*Fetch the url of the focused Chrome top window.*/
const std::string fetch_url();

/*Store Bookmark on disk.*/
bool store_bookmark(const Bookmark& bookmark, const fs::path& path);

} // namespace bbm
} // namespace barn
