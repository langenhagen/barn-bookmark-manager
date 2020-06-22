/* Implementation for fetch.hpp.

author: andreasl
*/
#include "fetch.hpp"

#include <algorithm>
#include <string>

namespace barn {
namespace bbm {

SearchBookmarkDialog::SearchBookmarkDialog(x11::App& app) : x11::Dialog(app)
{}

void SearchBookmarkDialog::draw() {
    constexpr static const int win_width = 200;
    constexpr static const int win_height = 50;
    if (!is_initalized) {
        app.resize_window(win_height, win_width);
        is_initalized = true;
    }

    const std::string title = "Barn's Bookmark Manager";
    const int title_padding = std::max((win_width - title.length())/2, 1UL);

    app.draw_text(app.fc_text, title, 1, title_padding);
}

x11::AppState SearchBookmarkDialog::handle_key_press(XEvent& evt) {
    switch(evt.xkey.keycode) {
        case 9: /*esc*/
            return x11::AppState::EXIT;
        case 36: /*ctrl + enter*/
            if (this->app.is_ctrl_pressed()) {
                return x11::AppState::EXIT;
            }
            break;
    }
    return x11::AppState::KEEP_RUNNING;
}
x11::AppState SearchBookmarkDialog::handle_key_release(XEvent& evt) {
    // TODO
    return x11::AppState::KEEP_RUNNING;
}

} // namespace bbm
} // namespace barn
