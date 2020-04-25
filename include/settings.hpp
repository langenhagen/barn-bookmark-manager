/* Common settings-related functionality.

author: andreasl
*/
#pragma once

#include "log.hpp"

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <exception>
#include <experimental/filesystem>
#include <fstream>
#include <string>

namespace barn {
namespace bbm {

/*Write the default settings to the given file paths.*/
bool write_default_settings(const std::string& path) {
    namespace fs = std::experimental::filesystem;

    const auto directory = fs::path(path).remove_filename();
    try {
        fs::create_directories(directory);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not create directory \"" << directory << "\":\n"
            << e.what() << std::endl;
        exit(1);
    }
    YAML::Node node;
    const auto bookmarks_path = fs::path(std::getenv("HOME")) / ".config/barn-bookmarks/";
    node["bookmarks_root_path"] = bookmarks_path.string();  /*root directory to the bookmarks*/
    node["editor"] = "vim";  /*text editor to drop in for modification of bookmarks*/
    node["open_browser_command"] = "xdg-open";  /*command to open a hyperlink*/

    /*control the add bookmarks process; reorder or remove elements*/
    node["add_bookmark_sequence"].push_back("ask_for_path");
    node["add_bookmark_sequence"].push_back("ask_for_tags");
    node["add_bookmark_sequence"].push_back("ask_for_comment");
    node["add_bookmark_sequence"].push_back("ask_for_rating");

    std::ofstream fout(path);
    fout << node;
    if (!fout) {
        log(ERROR) << "Could not write settings to file: " << path << std::endl;
        return false;
    }
    return true;
}

/*Common template function to load settings from file.*/
template<class SettingsType, void LoadFunction(const std::string&, SettingsType&)>
void load_settings(const std::string& path, SettingsType& settings) {
    namespace fs = std::experimental::filesystem;
    try {
        LoadFunction(path, settings);
    } catch (const YAML::BadFile& e) {
        log(ERROR) << "Could not read file \"" << path << "\"" << std::endl;
        try {
            if (!fs::exists(path)) {
                log(INFO) << "Creating file \"" << path << "\"..." << std::endl;
                write_default_settings(path);
                LoadFunction(path, settings);
            }
        } catch (const std::exception& e) {
            log(ERROR) << e.what() << std::endl;
            exit(1);
        }
    } catch (const YAML::Exception& e) {
        log(ERROR) << e.what() << std::endl;
        exit(1);
    }
}

} // namespace bbm
} // namespace barn
