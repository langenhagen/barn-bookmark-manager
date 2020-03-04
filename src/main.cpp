#include <iostream>
#include <string>

#include <version_info.hpp>

int main(int argc, const char* argv[]) {
    std::string build_timestamp = bbm::get_build_timestamp();
    std::cout << "Hello, World!\nVersion: " << build_timestamp << std::endl;
    return 0;
}
