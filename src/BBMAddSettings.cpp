/* Implementation for BBMAddSettings.hpp.

author: andreasl
*/
#include "BBMAddSettings.hpp"

#include "settings_util.hpp"

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <unordered_map>

namespace barn {
namespace bbm {

/*Load AddSettings from file.*/
static void load(const std::string& path, AddSettings* settings) {
    const YAML::Node& node = YAML::LoadFile(path);
    settings->bookmarks_root_path = node["bookmarks_root_path"].as<std::string>();
    settings->editor = node["editor"].as<std::string>();

    settings->dialog_sequence.clear();
    static const std::unordered_map<std::string, AddBookmarkDialog> str_to_dialog {
        {"ask_for_comment", AddBookmarkDialog::ask_for_comment},
        {"ask_for_path", AddBookmarkDialog::ask_for_path},
        {"ask_for_rating", AddBookmarkDialog::ask_for_rating},
        {"ask_for_tags", AddBookmarkDialog::ask_for_tags}
    };
    const auto& sequence_node = node["add_bookmark_dialog_sequence"];
    for(const auto& node : sequence_node) {
        try {
            const auto dialog = str_to_dialog.at(node.as<std::string>());
            settings->dialog_sequence.push_back(dialog);
        } catch (const std::out_of_range& e) {
            std::cerr
                << "Warning: Unknown setting for add bookmark dialog \"" << node << "\"" << std::endl;
        }
    }
}

/*Load AddSettings from file.*/
void load_settings(const std::string& path, AddSettings* settings) {
    load_settings<AddSettings, load>(path, settings);
}

} // namespace bbm
} // namespace barn
