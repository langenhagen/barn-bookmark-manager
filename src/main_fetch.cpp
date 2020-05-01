/* Entry point of Barn's Bookmark Manager Fetch bookmark program.

author: andreasl
*/
#include "cli.hpp"
#include "fetch.hpp"
#include "fetch_settings.hpp"

int main(int argc, const char* argv[]) {
    using namespace ::barn::bbm;

    const auto options = parse_options(argc, argv);
    const auto settings = load_settings(options.settings_path);

    x11::App app(settings);
    app.run();

    return exitcode::SUCCESS;
}
