/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "add.hpp"
#include "add_settings.hpp"
#include "bookmark.hpp"
#include "cli.hpp"
#include "datetime.hpp"
#include "log.hpp"

#include <iostream>
#include <vector>
#include <utility>

int main(int argc, const char* argv[]) {
    const auto options = ::barn::bbm::parse_options(argc,argv);
    ::barn::bbm::AddSettings settings;
    ::barn::bbm::load_settings(options.settings_path, settings);

    std::cout << "settings:"
        << "\n  bookmark-root-path: " << settings.bookmarks_root_path
        << "\n  editor: " << settings.editor
        << "\n  dialog_sequence: " << settings.dialog_sequence.size() << " items.";
    for (const auto& dialog : settings.dialog_sequence) {
        std::cout << "\n    " << (int)dialog;
    }

    std::string url;
    std::string title;
    if( !::barn::bbm::fetch_url_and_title(url, title)) {
        ::barn::bbm::log(::barn::bbm::ERROR) << "Could not fetch url and title." << std::endl;
        exit(::barn::bbm::exitcode::WRONG_INPUT);
    }

    ::barn::bbm::Bookmark bookmark{
        std::move(url),
        ::barn::bbm::DateTime::now(),
        ::barn::bbm::DateTime::now(),
        std::move(title),
        0,
        {"hi", "there!"},
        "Here goes\na comment.\nJustfor you :)",
        nullptr
    };

    ::barn::bbm::save_bookmark(std::move(bookmark), settings.bookmarks_root_path / "subpath");

    return ::barn::bbm::exitcode::SUCCESS;
}
