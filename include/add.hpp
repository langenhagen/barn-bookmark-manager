/* Business logic for BBM Add app.

author: andreasl
*/
#pragma once

#include <string>

namespace barn {
namespace bbm {

class AddSettings;
class Bookmark;

/*Fetch the url of the focused Chrome top window.*/
const std::string fetch_url();

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const AddSettings&);

} // namespace bbm
} // namespace barn
