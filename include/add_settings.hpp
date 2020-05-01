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

/*Dialogues for the add bookmark application flow.*/
enum class Dialog {
    ask_for_comment  /*bookmark comment dialog*/,
    ask_for_path  /*bookmark folder dialog*/,
    ask_for_rating  /*bookmark rating dialog*/,
    ask_for_tags  /*bookmark tags dialog*/
};

/*Settings for the BBM Add app.*/
struct Settings {
    using DialogVector = std::vector<Dialog>;

    fs::path bookmarks_root_path;  /*Root directory to the bookmarks.*/
    std::string editor;  /*Text editor to drop in for modification of bookmarks.*/
    DialogVector dialog_sequence;  /*Ordered selection of applied dialogS*/
};

/*Load Settings from file.*/
Settings load_settings(const fs::path& path);

} // namespace bbm
} // namespace barn
