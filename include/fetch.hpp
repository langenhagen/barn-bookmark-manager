/* Business logic for BBM Fetch app.

author: andreasl
*/
#pragma once

#include "bookmark.hpp"
#include "x_app.hpp"

namespace barn {
namespace bbm {

/*The BBM Fetch app context.*/
struct Context {
    Bookmark bookmark;  /*The Bookmark.*/
    bool do_open = false;  /*Whether or not to open the bookmark*/
};

struct SearchBookmarkDialog : x11::Dialog {
    SearchBookmarkDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
    x11::AppState handle_key_release(XEvent& evt);
};

} // namespace bbm
} // namespace barn
