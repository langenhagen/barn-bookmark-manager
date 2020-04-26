/* Entry point of Barn's Bookmark Manager Fetch bookmark program.

author: andreasl
*/
#include "fetch_settings.hpp"
#include "cli.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    const auto options = ::barn::bbm::parse_options(argc, argv);
    ::barn::bbm::FetchSettings settings;
    ::barn::bbm::load_settings(options.settings_path, settings);

    std::cout << "settings:"
        << "\n  bookmark-root-path: " << settings.bookmarks_root_path
        << "\n  editor: " << settings.editor
        << "\n  open-browser-command: " << settings.open_browser_command;

    return 0;
}
