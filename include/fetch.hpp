/* Business logic for BBM Fetch app.

author: andreasl
*/
#pragma once

#include "x_app.hpp"

namespace barn {
namespace bbm {

struct SearchBookmarkDialog : x11::Dialog {
    SearchBookmarkDialog(x11::App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

} // namespace bbm
} // namespace barn
