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
int SearchBookmarkDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}
int SearchBookmarkDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

} // namespace bbm
} // namespace barn
