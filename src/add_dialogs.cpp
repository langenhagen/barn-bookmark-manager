/* Implementation for add_dialogs.hpp.

author: andreasl
*/
#include "add_dialogs.hpp"

#include "bookmark.hpp"

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include <memory>

namespace barn {
namespace bbm {
namespace x11 {

AddCommentDialog::AddCommentDialog(App& app) : Dialog(app)
{}

void AddCommentDialog::draw() {
    // TODO
}
int AddCommentDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}
int AddCommentDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

AddPathDialog::AddPathDialog(App& app) : Dialog(app)
{}

void AddPathDialog::draw() {
    // TODO
}
int AddPathDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}

int AddPathDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

AddRatingDialog::AddRatingDialog(App& app) : Dialog(app)
{}

void AddRatingDialog::draw() {
    // TODO
}

int AddRatingDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}

int AddRatingDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

AddTagsDialog::AddTagsDialog(App& app) : Dialog(app)
{}

void AddTagsDialog::draw() {
    // TODO
}

int AddTagsDialog::handle_key_press(XEvent& evt) {
    // TODO
}

int AddTagsDialog::handle_key_release(XEvent& evt) {
    // TODO
}

} // namespace x11
} // namespace bbm
} // namespace barn