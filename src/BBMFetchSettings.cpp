/* Implementation for BBMFetchSettings.hpp.

author: andreasl
*/
#include "BBMFetchSettings.hpp"

#include "settings_util.hpp"

#include <yaml-cpp/yaml.h>

#include <string>

namespace barn {
namespace bbm {

static void load(const std::string& path, FetchSettings* settings) {
    const YAML::Node& node = YAML::LoadFile(path);
    settings->bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings->editor = node["editor"].as<std::string>();
    settings->open_browser_command = node["open_browser_command"].as<std::string>();
}

/*Load FetchSettings from file.*/
void load_settings(const std::string& path, FetchSettings* settings) {
    load_settings<FetchSettings, load>(path, settings);
}

} // namespace bbm
} // namespace barn
