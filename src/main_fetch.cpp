/* Entry point of Barn's Bookmark Manager Fetch bookmark program.

author: andreasl
*/
#include "fetch_settings.hpp"
#include "cli.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    const auto options = ::barn::bbm::parse_options(argc, argv);
    auto settings = ::barn::bbm::load_settings(options.settings_path);

    return ::barn::bbm::exitcode::SUCCESS;
}
