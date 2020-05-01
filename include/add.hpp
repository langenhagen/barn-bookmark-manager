/* Business logic for BBM Add app.

author: andreasl
*/
#pragma once

#include "add_settings.hpp"
#include "x_app.hpp"

#include <experimental/filesystem>
#include <memory>
#include <string>

#include <X11/Xlib.h>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

struct Bookmark;

/*Fetch the url and the website tile from the focused Chrome top window.*/
bool fetch_url_and_title(std::string& url, std::string& title);

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const fs::path& directory);

/*Add bookmark x11 window application.*/
struct AddBookmarkApp : x11::App {
    AddSettings settings;  /*Application settings.*/

    AddBookmarkApp(const AddSettings& settings); /*Constructor.*/
};

struct AddCommentDialog : x11::Dialog {
    AddCommentDialog(x11::App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddPathDialog : x11::Dialog {
    AddPathDialog(x11::App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddRatingDialog : x11::Dialog {
    AddRatingDialog(x11::App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

struct AddTagsDialog : x11::Dialog {
    AddTagsDialog(x11::App& app);
    void draw();
    int handle_key_press(XEvent& evt);
    int handle_key_release(XEvent& evt);
};

} // namespace bbm
} // namespace barn
