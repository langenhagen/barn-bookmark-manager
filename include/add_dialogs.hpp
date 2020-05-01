/* BBM Add dialogs.

author: andreasl
*/
#pragma once

#include "x11.hpp"

#include <memory>

#include <X11/Xlib.h>

namespace barn {
namespace bbm {

namespace x11 {

struct AddCommentDialog : Dialog {
    AddCommentDialog(App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddPathDialog : Dialog {
    AddPathDialog(App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddRatingDialog : Dialog {
    AddRatingDialog(App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddTagsDialog : Dialog {
    AddTagsDialog(App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

} // namespace x11
} // namespace bbm
} // namespace barn