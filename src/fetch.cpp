/* Implementation for fetch.hpp.

author: andreasl
*/
#include "fetch.hpp"

namespace barn {
namespace bbm {

SearchBookmarkDialog::SearchBookmarkDialog(x11::App& app) : x11::Dialog(app)
{}

void SearchBookmarkDialog::draw() {
    // TODO
}
x11::AppState SearchBookmarkDialog::handle_key_press(XEvent& evt) {
    // TODO
    return x11::AppState::EXIT;
}
x11::AppState SearchBookmarkDialog::handle_key_release(XEvent& evt) {
    // TODO
    return x11::AppState::EXIT;
}

} // namespace bbm
} // namespace barn
