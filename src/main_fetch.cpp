/* Entry point of Barn's Bookmark Manager Fetch bookmark program.

author: andreasl
*/
#include "cli.hpp"
#include "fetch.hpp"
#include "settings.hpp"

#include <memory>

int main(int argc, const char* argv[]) {
    using namespace ::barn::bbm;

    const auto options = parse_options(argc, argv);
    const auto settings = std::make_shared<Settings>(load_settings(options.settings_path));
    auto context = std::make_shared<Context>();
    x11::App app(settings, context);
    app.run();

    return exitcode::SUCCESS;
}
