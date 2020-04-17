/* Entry point of Barn's Bookmark Manager Fetch bookmark program.

author: andreasl
*/
#include "version_info.hpp"

#include <iostream>


int main(int argc, const char* argv[]) {
    std::cout << "Fetch Hello!\n" << std::endl;

    // TODO command line option parsing: --help print help and die
    // TODO if command line options other than help: short help and die

    // TODO add...

    const std::string app_version = ::bbm::get_app_version();
    const std::string build_timestamp = ::bbm::get_build_timestamp();

    std::cout << "app version: " <<  app_version << std::endl;
    std::cout << "build timestamp: " << build_timestamp << std::endl;

    std::cout << "\nBye!" << std::endl;
    return 0;
}
