/* Business logic for BBM Add app.

author: andreasl
*/
#pragma once

#include "bookmark.hpp"
#include "x_app.hpp"

#include <experimental/filesystem>
#include <string>

#include <X11/Xlib.h>

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

/*The BBM Add app context.*/
struct Context {
    Bookmark bookmark;  /*The bookmark.*/
    bool keep_querystring = true;  /*Whether or not to keep a possible querystring.*/
    bool do_store = true;  /*Whether or not to store the bookmark.*/
    fs::path bookmark_dir;  /*Directory where to store the bookmark.*/
};

/*Fetch the url and the website tile from the focused Chrome top window.*/
bool fetch_url_and_title(std::string& url, std::string& title);

/*Add application dialogs from dialog sequence settings.*/
void add_dialogs_to_app(x11::App& app);

/*Get a url without querystring.*/
std::string remove_querystring(const std::string& url);

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const fs::path& directory);

struct ReviewURLDialog : x11::Dialog {
    std::string url_without_querystring;

    ReviewURLDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
};

struct AddPathDialog : x11::Dialog {
    x11::TextBox txt_path;

    AddPathDialog(x11::App& app);
    void draw();
    x11::AppState handle_key_press(XEvent& evt);
};

struct AddCommentDialog : x11::Dialog {
    AddCommentDialog(x11::App& app);
    void draw();
};

struct AddRatingDialog : x11::Dialog {
    AddRatingDialog(x11::App& app);
    void draw();
};

struct AddTagsDialog : x11::Dialog {
    AddTagsDialog(x11::App& app);
    void draw();
};

} // namespace bbm
} // namespace barn
