/* Business logic for BBM Add app.

author: andreasl
*/
#pragma once

#include "bookmark.hpp"
#include "x_app.hpp"

#include <experimental/filesystem>
#include <memory>
#include <string>

#include <X11/Xlib.h>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

/*The BBM Add app context.*/
struct Context {
    Bookmark bookmark;  /*The bookmark.*/
    bool do_store = false;  /*Whether or not to store the bookmark.*/
};

/*Fetch the url and the website tile from the focused Chrome top window.*/
bool fetch_url_and_title(std::string& url, std::string& title);

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const fs::path& directory);

struct AddCommentDialog : x11::Dialog {
    AddCommentDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
    x11::AppState handle_key_release(XEvent& evt);
};

struct AddPathDialog : x11::Dialog {
    AddPathDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
    x11::AppState handle_key_release(XEvent& evt);
};

struct AddRatingDialog : x11::Dialog {
    AddRatingDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
    x11::AppState handle_key_release(XEvent& evt);
};

struct AddTagsDialog : x11::Dialog {
    AddTagsDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
    x11::AppState handle_key_release(XEvent& evt);
};

} // namespace bbm
} // namespace barn
