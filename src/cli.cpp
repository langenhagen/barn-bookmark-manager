/* Implementation for cli.hpp.

author: andreasl
*/
#include "cli.hpp"
#include "version_info.hpp"

#include <cstring>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <utility>

namespace barn {
namespace bbm {

/*Print a help message to stdout.*/
void show_help(const std::string& app_name) {
    std::cout << "Usage:\n" << app_name << " <path-to-settings>" << std::endl;
}

void show_version(const std::string& app_name) {
    constexpr auto version = ::barn::bbm::get_app_version();
    constexpr auto build_timestamp = ::barn::bbm::get_build_timestamp();
    std::cout << app_name << " version: " << version << " build: " << build_timestamp << std::endl;
}

/*Parse command line options.*/
Options parse_options(int argc, const char* argv[]) {
    namespace fs = std::experimental::filesystem;

    const auto app_name = fs::path(argv[0]).filename().string();
    if (argc > 2) {
        show_help(std::move(app_name));
        exit(exitcode::WRONG_CLI_ARGUMENTS);
    }
    if (argc == 1) {
        const fs::path path = fs::path(std::getenv("HOME"))
            / ".config" / "barn-bookmarks" / "settings.yaml";
        return Options{std::move(path)};
    }
    if (std::strcmp(argv[1], "--version") == 0 || std::strcmp(argv[1], "-v") == 0) {
        show_version(std::move(app_name));
        exit(exitcode::SUCCESS);
    }
    if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
        show_help(std::move(app_name));
        exit(exitcode::SUCCESS);
    }
    return Options{argv[1]};
}

} // namespace bbm
} // namespace barn
