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

/*Load AddSettings from file.*/
static AddSettings load(const fs::path& path) {
    const YAML::Node& node = YAML::LoadFile(path.string());
    AddSettings settings;
    settings.bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings.editor = node["editor"].as<std::string>();

    settings.dialog_sequence.clear();
    static const std::unordered_map<std::string, AddBookmarkDialog> str_to_dialog {
        {"ask_for_comment", AddBookmarkDialog::ask_for_comment},
        {"ask_for_path", AddBookmarkDialog::ask_for_path},
        {"ask_for_rating", AddBookmarkDialog::ask_for_rating},
        {"ask_for_tags", AddBookmarkDialog::ask_for_tags}
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

/*Load AddSettings from file.*/
AddSettings load_settings(const fs::path& path) {
    return load_settings<AddSettings, load>(path);
}

} // namespace bbm
} // namespace barn
