/* BBM Add dialogs.

author: andreasl
*/
#pragma once

#include "x11.hpp"

#include <memory>

#include <X11/Xlib.h>

namespace barn {
namespace bbm {

class Bookmark;

namespace x11 {

struct AddCommentDialog : Dialog {
    AddCommentDialog(std::shared_ptr<Bookmark> bookmark);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddPathDialog : Dialog {
    AddPathDialog(std::shared_ptr<Bookmark> bookmark);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddRatingDialog : Dialog {
    AddRatingDialog(std::shared_ptr<Bookmark> bookmark);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddTagsDialog : Dialog {
    AddTagsDialog(std::shared_ptr<Bookmark> bookmark);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

} // namespace x11
} // namespace bbm
} // namespace barn