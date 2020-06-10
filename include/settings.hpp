/* Common settings-related functionality.

author: andreasl
*/
#pragma once

#include "cli.hpp"
#include "log.hpp"

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <exception>
#include <experimental/filesystem>
#include <fstream>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

/*Write the default settings to the given file paths.*/
bool write_default_settings(const fs::path& path) {
    const auto directory(fs::path(path).remove_filename());
    try {
        fs::create_directories(directory);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not create directory " << directory << ":\n"
            << e.what() << std::endl;
        exit(exitcode::SYSTEM_ERROR);
    }
    YAML::Node node;
    const auto bookmarks_path = directory / "bookmarks";
    node["bookmarks_root_path"] = bookmarks_path.string();  /*root directory to the bookmarks*/
    node["editor"] = "vim";  /*text editor to drop in for modification of bookmarks*/
    node["open_browser_command"] = "xdg-open";  /*command to open a hyperlink*/

    /*control the add bookmarks process; reorder or remove elements*/
    node["add_bookmark_sequence"].push_back("review_url");
    node["add_bookmark_sequence"].push_back("ask_for_path");
    node["add_bookmark_sequence"].push_back("ask_for_tags");
    node["add_bookmark_sequence"].push_back("ask_for_comment");
    node["add_bookmark_sequence"].push_back("ask_for_rating");

    std::ofstream out(path);
    out << node;
    if (!out) {
        log(ERROR) << "Could not write settings to file: " << path << std::endl;
        return false;
    }
    return true;
}

/*Common template function to load settings from file.*/
template<class SettingsType, SettingsType LoadFunction(const fs::path&)>
SettingsType load_settings(const fs::path& path) {
    try {
        return LoadFunction(path);
    } catch (const YAML::BadFile& e) {
        log(ERROR) << "Could not read file " << path << std::endl;
        try {
            if (!fs::exists(path)) {
                log(INFO) << "Creating file " << path << "..." << std::endl;
                write_default_settings(path);
                return LoadFunction(path);
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
