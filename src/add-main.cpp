/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "version_info.hpp"
#include "BBMAddSettings.hpp"

#include <cstdlib>
#include <experimental/filesystem>
#include <iostream>

int main(int argc, const char* argv[]) {
    namespace fs = std::experimental::filesystem;

    std::cout << "Add Hello!\n" << std::endl;

    // TODO command line option parsing: --help print help and die
    // TODO if command line options other than help: short help and die

    // TODO add...

    const std::string app_version = ::barn::bbm::get_app_version();
    const std::string build_timestamp = ::barn::bbm::get_build_timestamp();

    std::cout << "app version: " <<  app_version << std::endl;
    std::cout << "build timestamp: " << build_timestamp << std::endl;

    ::barn::bbm::AddSettings settings;
    const fs::path settings_path =
        fs::path(std::getenv("HOME")) / ".config/barn-bookmarks/settings.yaml";
    ::barn::bbm::load_settings(settings_path.string(), &settings);

    std::cout << "settings:"
        << "\n  bookmark-root-path: " << settings.bookmarks_root_path
        << "\n  editor: " << settings.editor
        << "\n  dialog_sequence: " << settings.dialog_sequence.size() << " items.";
    for(const auto& dialog : settings.dialog_sequence) {
        std::cout << "\n    " << (int)dialog;
    }

    std::cout << "\nBye!" << std::endl;
    return 0;
}
