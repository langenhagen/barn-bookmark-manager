/* Implementation for fetch_settings.hpp.

author: andreasl
*/
#include "fetch_settings.hpp"

#include "settings.hpp"

#include <yaml-cpp/yaml.h>

#include <experimental/filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

namespace {

static Settings load(const fs::path& path) {
    const YAML::Node& node = YAML::LoadFile(path.string());
    Settings settings;
    settings.bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings.editor = node["editor"].as<std::string>();
    settings.open_browser_command = node["open_browser_command"].as<std::string>();
    return settings;
}

} // namespace

Settings load_settings(const fs::path& path) {
    return load_settings<Settings, load>(path);
}

} // namespace bbm
} // namespace barn
