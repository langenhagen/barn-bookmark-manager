/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "add.hpp"
#include "add_settings.hpp"
#include "bookmark.hpp"
#include "cli.hpp"
#include "datetime.hpp"
#include "log.hpp"

#include <utility>
#include <vector>

int main(int argc, const char* argv[]) {
    using namespace ::barn::bbm;

    const auto options = parse_options(argc,argv);
    const auto settings = std::make_shared<Settings>(load_settings(options.settings_path));
    auto context = std::make_shared<Context>();

    x11::App app(settings, context);
    app.run();

    return exitcode::SUCCESS;
}
