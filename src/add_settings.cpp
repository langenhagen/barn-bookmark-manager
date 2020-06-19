/* Implementation for add_settings.hpp.

author: andreasl
*/
#include "add_settings.hpp"

#include "settings.hpp"
#include "log.hpp"

#include <yaml-cpp/yaml.h>

#include <experimental/filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

namespace {

/*Load Settings from file.*/
static Settings load(const fs::path& path) {
    const YAML::Node& node = YAML::LoadFile(path.string());
    Settings settings;
    settings.bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings.download_websites = node["download_websites"].as<bool>();
    settings.editor = node["editor"].as<std::string>();

    settings.dialog_sequence.clear();
    static const std::unordered_map<std::string, Dialog> str_to_dialog {
        {"review_url", Dialog::review_url},
        {"ask_for_comment", Dialog::ask_for_comment},
        {"ask_for_path", Dialog::ask_for_path},
        {"ask_for_rating", Dialog::ask_for_rating},
        {"ask_for_tags", Dialog::ask_for_tags}
    };
    const auto& sequence_node(node["add_bookmark_sequence"]);
    for (const auto& node : sequence_node) {
        try {
            const auto dialog = str_to_dialog.at(node.as<std::string>());
            settings.dialog_sequence.emplace_back(dialog);
        } catch (const std::out_of_range& e) {
            log(WARN) << "Unknown setting for add bookmark dialog \"" << node << "\"" << std::endl;
        }
    }
    return settings;
}

} // namespace

Settings load_settings(const fs::path& path) {
    return load_settings<Settings, load>(path);
}

} // namespace bbm
} // namespace barn
