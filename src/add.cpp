/* Implementation for add.hpp.

author: andreasl
*/
#include "add.hpp"

#include "add_settings.hpp"
#include "bookmark.hpp"
#include "log.hpp"
#include "x_copy_paste.hpp"

#include <chrono>
#include <cstring>
#include <experimental/filesystem>
#include <functional>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

extern "C" {
#include <xdo.h>
}

namespace barn {
namespace bbm {

namespace {

/*Create a hash to a given string.*/
static const std::string generate_hash(const std::string& str) {
    static const std::hash<std::string> hash_function;
    const size_t hash = hash_function(str) % 0xffffff;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(6)  << std::hex << hash;
    return ss.str();
}

} // namespace

/*Fetch the url of the focused Chrome top window.*/
const std::string fetch_url() {
    std::unique_ptr<xdo_t, decltype(&xdo_free)> xdo(xdo_new(nullptr), &xdo_free);

    Window win;
    xdo_get_active_window(xdo.get(), &win);

    unsigned char* win_name;
    int name_len;
    int win_type;
    xdo_get_window_name(xdo.get(), win, &win_name, &name_len, &win_type);

    constexpr const unsigned char chrome_win_suffix[17] = " - Google Chrome";
    if (std::memcmp(win_name + name_len - 16, chrome_win_suffix, 16)) {
        log(ERROR) << "Window \"" << win_name << "\" is not a Google Chrome window." << std::endl;
        return "";
    }

    /*focus the chrome address bar*/
    xdo_send_keysequence_window(xdo.get(), win, "Control_L+l", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); /*sleeping seems necessary*/

    /*copy the contents*/
    xdo_send_keysequence_window(xdo.get(), win, "Control_L+c", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    return ::barn::x11::cp::get_text_from_clipboard();
}

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const AddSettings& settings) {
    namespace fs = std::experimental::filesystem;

    const fs::path root_path = settings.bookmarks_root_path;
    const std::string hash = generate_hash(bookmark.url + bookmark.created.str());
    fs::path bookmark_folder(root_path / hash);
    try {
        if (fs::exists(bookmark_folder)) {
            log(ERROR) << "Bookmark folder \"" << bookmark_folder << "\" does already exist."
                << std::endl;
            return false;
        }
        fs::create_directories(bookmark_folder);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not read or create directory \"" << root_path << "\"\n"
            << e.what() << std::endl;
        return false;
    }

    return true;
}

} // namespace bbm
} // namespace barn
