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
    using namespace ::barn::bbm;

    const auto options = parse_options(argc,argv);
    const auto settings = load_settings(options.settings_path);

    std::string url;
    std::string title;
    if( !fetch_url_and_title(url, title)) {
        log(ERROR) << "Could not fetch url and title." << std::endl;
        exit(exitcode::WRONG_INPUT);
    }
    Bookmark bookmark{std::move(url), DateTime::now(), DateTime::now(), std::move(title)};
    save_bookmark(std::move(bookmark), settings.bookmarks_root_path / "subpath");

    x11::App app;
    app.run();

    return exitcode::SUCCESS;
}
