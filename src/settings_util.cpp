/* Implementation for settings_util.cpp.

author: andreasl
*/
#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <string>

/*Write the default settings to the given file paths.*/
bool write_default_settings(const std::string& path) {
    namespace fs = std::experimental::filesystem;

    const fs::path bookmarks_path = fs::path(std::getenv("HOME")) / ".config/barn-booksmarks/";

    YAML::Node node;
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
    if(!fout) {
        // TODO refactor into another layer?
        std::cerr << "Error: could not write settings to file: " << path << std::endl;
        return false;
    }
    return true;
}
