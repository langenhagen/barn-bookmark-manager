/* Implementation for BBMFetchSettings.hpp.

author: andreasl
*/
#include "BBMFetchSettings.hpp"

#include <yaml-cpp/yaml.h>

#include <string>

namespace barn {
namespace bbm {

/*Load FetchSettings from file.*/
bool load_settings(const std::string& path, FetchSettings* settings) {
    const YAML::Node& node = YAML::LoadFile(path);
    settings->bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings->editor = node["editor"].as<std::string>();
    return true;
}

} // namespace bbm
} // namespace barn
