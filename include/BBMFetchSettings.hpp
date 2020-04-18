/* Settings structure for BBM Fetch app and related functionality.

author: andreasl
*/
#include <string>

namespace barn {
namespace bbm {

/*Settings for the BBM Add app.*/
struct FetchSettings {
    std::string bookmarks_root_path;  /*root directory to the bookmarks*/
    std::string config_path;  /*path to the settings yaml file*/
    std::string editor;  /*text editor to drop in for modification of bookmarks*/
};

/*Load FetchSettings from file.*/
int load_settings(const char* path, FetchSettings* settings);

} // namespace bbm
} // namespace barn
