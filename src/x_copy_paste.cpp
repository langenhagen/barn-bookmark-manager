/* Implementation for x_copy_paste.hpp.

author: andreasl
*/
#include "x_copy_paste.hpp"

#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

namespace barn {
namespace x11 {
namespace cp {

namespace {

/*Call popen() & pclose() and return its output.*/
std::string call_popen(const char* cmd) {
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Error: call to popen() failed");
    }
    std::array<char, 128> buf;
    while (std::fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
        result += buf.data();
    }
    return result;
}

} // namespace

/*Write the given string to the x window clipboard.*/
void write_to_clipboard(const std::string& str) {
    const std::string cmd = "printf -- '" + str + "' | xclip -selection clipboard";
    system(cmd.c_str());
}

/*Return a string from the x window clipboard.*/
std::string get_text_from_clipboard() {
    return call_popen("xclip -selection clipboard -o");
}

} // namespace cp
} // namespace x11
} // namespace barn
