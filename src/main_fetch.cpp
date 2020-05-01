/* Entry point of Barn's Bookmark Manager Fetch bookmark program.

author: andreasl
*/
#include "fetch_settings.hpp"
#include "cli.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    using namespace ::barn::bbm;

    const auto options = parse_options(argc, argv);
    const auto settings = load_settings(options.settings_path);

    return exitcode::SUCCESS;
}
