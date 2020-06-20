/* Implementation for settings.hpp.

author: andreasl
*/
#include "settings.hpp"

#include "cli.hpp"
#include "log.hpp"

#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace barn {
namespace bbm {

namespace {

/*Convert settings from yaml to a Settings object.*/
Settings yaml_to_settings(const YAML::Node& node, const DefaultSettings& defaults) {
    Settings settings;
    const std::unordered_map<std::string, Dialog> str_to_dialog {
        {"ask_for_comment", Dialog::ask_for_comment},
        {"ask_for_path", Dialog::ask_for_path},
        {"ask_for_rating", Dialog::ask_for_rating},
        {"ask_for_tags", Dialog::ask_for_tags},
        {"review_url", Dialog::review_url}
    };
    std::vector<std::string> str_dialog_sequence = node["add_bookmark_dialog_sequence"]
        .as<std::vector<std::string>>(defaults.add_bookmark_dialog_sequence);
    for (const std::string& dialog_str : str_dialog_sequence) {
        try {
            const Dialog dialog = str_to_dialog.at(dialog_str);
            settings.add_bookmark_dialog_sequence.emplace_back(dialog);
        } catch (const std::out_of_range& e) {
            log(WARN) << "Warning: string value not in enum \"" << dialog_str << "\"" << std::endl;
        }
    }
    settings.bookmarks_root_path = node["bookmarks_root_path"]
        .as<std::string>(defaults.bookmarks_root_path);
    settings.download_websites = node["download_websites"]
        .as<bool>(defaults.download_websites);
    settings.editor = node["editor"]
        .as<std::string>(defaults.editor);
    settings.open_browser_command = node["open_browser_command"]
        .as<std::string>(defaults.open_browser_command);

    return settings;
}

/*Write the default settings to the given file paths.*/
void write_default_settings(const fs::path& path) {
    const auto directory(fs::path(path).remove_filename());
    try {
        fs::create_directories(directory);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not create directory " << directory << ":\n" << e.what() << std::endl;
        exit(exitcode::SYSTEM_ERROR);
    }
    YAML::Node node;
    const DefaultSettings defaults{directory / "bookmarks"};
    node["bookmarks_root_path"] = defaults.bookmarks_root_path.string();
    node["download_websites"] = defaults.download_websites;
    node["editor"] = defaults.editor;
    node["open_browser_command"] = defaults.open_browser_command;
    for (const std::string& item : defaults.add_bookmark_dialog_sequence) {
        node["add_bookmark_dialog_sequence"].push_back(item);
    }

    std::ofstream out(path);
    out << node;
    if (!out) {
        log(ERROR) << "Could not write settings to file: " << path << std::endl;
        exit(exitcode::SYSTEM_ERROR);
    }
}

} // namespace

/*Load settings from file.*/
Settings load_settings(const fs::path& path) {
    const auto directory(fs::path(path).remove_filename());
    const DefaultSettings defaults{directory / "bookmarks"};
    try {
        const YAML::Node node = YAML::LoadFile(path);
        return yaml_to_settings(node, defaults);
    } catch (const YAML::BadFile& e) {
        log(ERROR) << "Could not read file " << path << std::endl;
        try {
            if (!fs::exists(path)) {
                log(INFO) << "Creating file " << path << "..." << std::endl;
                write_default_settings(path);
                return load_settings(path);
            }
        } catch (const std::exception& e) {
            log(ERROR) << e.what() << std::endl;
        }
    } catch (const YAML::Exception& e) {
        log(ERROR) << e.what() << std::endl;
    }
    exit(exitcode::SYSTEM_ERROR);
}

} // namespace bbm
} // namespace barn


