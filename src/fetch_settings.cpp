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

static FetchSettings load(const fs::path& path) {
    const YAML::Node& node = YAML::LoadFile(path.string());
    FetchSettings settings;
    settings.bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings.editor = node["editor"].as<std::string>();
    settings.open_browser_command = node["open_browser_command"].as<std::string>();
    return settings;
}

} // namespace

FetchSettings load_settings(const fs::path& path) {
    return load_settings<FetchSettings, load>(path);
}

} // namespace bbm
} // namespace barn
