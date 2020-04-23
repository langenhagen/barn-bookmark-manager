/* Implementation for add.hpp.

author: andreasl
*/
#include "add.hpp"

#include "util.hpp"
#include "x_copy_paste.hpp"

#include <chrono>
#include <cstring>
#include <memory>
#include <string>
#include <thread>

extern "C" {
#include <xdo.h>
}

namespace barn {
namespace bbm {

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
    if(std::memcmp(win_name + name_len - 16, chrome_win_suffix, 16)) {
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

} // namespace bbm
} // namespace barn
