/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "add.hpp"
#include "add_settings.hpp"
#include "bookmark.hpp"
#include "cli.hpp"
#include "datetime.hpp"
#include "log.hpp"
#include "x11.hpp"

#include <iostream>
#include <vector>
#include <utility>

int main(int argc, const char* argv[]) {
    const auto options = ::barn::bbm::parse_options(argc,argv);
    auto settings = ::barn::bbm::load_settings(options.settings_path);

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
        std::move(title)};
    ::barn::bbm::save_bookmark(std::move(bookmark), settings.bookmarks_root_path / "subpath");

    ::barn::bbm::x11::App app;
    app.run();

    return ::barn::bbm::exitcode::SUCCESS;
}
