/* Settings structure for BBM Add app and related functionality.

author: andreasl
*/
#include <string>
#include <vector>

namespace barn {
namespace bbm {

/*Dialogues for the add bookmark application flow.*/
enum class AddBookmarkDialog {
    ask_for_comment  /*bookmark comment dialog*/,
    ask_for_path  /*bookmark folder dialog*/,
    ask_for_rating  /*bookmark rating dialog*/,
    ask_for_tags  /*bookmark tags dialog*/
};

/*Settings for the BBM Add app.*/
struct AddSettings {
    std::string bookmarks_root_path;  /*root directory to the bookmarks*/
    std::string config_path;  /*path to the settings yaml file*/
    std::string editor;  /*text editor to drop in for modification of bookmarks*/
    std::vector<AddBookmarkDialog> dialog_sequence;  /*ordered selection of applied dialogs*/
};

/*Load AddSettings from file.*/
int load_settings(const char* path, AddSettings* settings);

} // namespace bbm
} // namespace barn
