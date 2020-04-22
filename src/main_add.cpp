/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "add_settings.hpp"
#include "cli.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    const auto options = ::barn::bbm::parse_options(argc,argv);
    ::barn::bbm::AddSettings settings;
    ::barn::bbm::load_settings(options.settings_path, &settings);

    std::cout << "settings:"
        << "\n  bookmark-root-path: " << settings.bookmarks_root_path
        << "\n  editor: " << settings.editor
        << "\n  dialog_sequence: " << settings.dialog_sequence.size() << " items.";
    for(const auto& dialog : settings.dialog_sequence) {
        std::cout << "\n    " << (int)dialog;
    }

    return 0;
}
