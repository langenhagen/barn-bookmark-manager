/* Settings structure for BBM Add app and related functionality.

author: andreasl
*/
#pragma once

#include <experimental/filesystem>
#include <string>
#include <vector>

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

/*Settings for the BBM Add app.*/
struct Settings {
    fs::path bookmarks_root_path;  /*Root directory to the bookmarks.*/
    bool download_websites;  /*Whether or not to download the website text.*/
    std::string editor;  /*Text editor to drop in for modification of bookmarks.*/
    std::vector<Dialog> dialog_sequence;  /*Ordered selection of applied dialog.*/
};

/*Load Settings from file.*/
Settings load_settings(const fs::path& path);

} // namespace bbm
} // namespace barn
