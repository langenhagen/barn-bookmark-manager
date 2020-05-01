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

    x11::App app;
    app.run();

    return exitcode::SUCCESS;
}
