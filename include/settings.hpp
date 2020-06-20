/* Common settings-related functionality.

author: andreasl
*/
#pragma once

#include <cstdlib>
#include <experimental/filesystem>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

/*Dialogs for the add bookmark application flow.*/
enum class Dialog {
    review_url  /*Review URL dialog.*/,
    ask_for_comment  /*Bookmark comment dialog.*/,
    ask_for_path  /*Bookmark folder dialog.*/,
    ask_for_rating  /*Bookmark rating dialog.*/,
    ask_for_tags  /*Bookmark tags dialog.*/
};

/*Settings for the BBM apps.*/
struct Settings {
    fs::path bookmarks_root_path;  /*Root directory to the bookmarks.*/
    bool download_websites;  /*Whether or not to download the website text.*/
    std::string editor;  /*Text editor to drop in for modification of bookmarks.*/
    std::string open_browser_command;  /*System command to open the browser.*/
    std::vector<Dialog> add_bookmark_dialog_sequence;  /*Ordered selection of dialogs.*/
};

/*Default settings for the BBM Apps.*/
struct DefaultSettings {
    const fs::path bookmarks_root_path =
        fs::path(std::getenv("HOME")) / ".config" / "barn-bookmarks" / "bookmarks";
    constexpr static const bool download_websites = true;
    const std::string editor = "vim";
    const std::string open_browser_command = "xdg-open";
    const std::vector<std::string> add_bookmark_dialog_sequence = {
        "review_url",
        "ask_for_path",
        "ask_for_tags",
        "ask_for_comment",
        "ask_for_rating"
    };
};

/*Load Settings from file.*/
Settings load_settings(const fs::path& path);

} // namespace bbm
} // namespace barn
