/* Implementation for add.hpp.

author: andreasl
*/
#include "add.hpp"

#include "add_settings.hpp"
#include "bookmark.hpp"
#include "log.hpp"
#include "x_app.hpp"
#include "x_copy_paste.hpp"

#include "yaml-cpp/yaml.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <experimental/filesystem>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>

extern "C" {
#include <xdo.h>
}

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

namespace {

/*Create random id to a given string.*/
const std::string generate_id() {
    std::stringstream ss;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 35);
    constexpr static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 6; ++i) {
        ss << alphanum[dis(gen)];
    }
    return ss.str();
}

/*Write Yaml to file.*/
bool write(const YAML::Emitter& yaml, const fs::path& directory) {
    const std::string filename = generate_id() + ".yml";
    const fs::path file = directory / filename;
    try {
        if (fs::exists(file)) {
            log(WARN) << "Bookmark file " << file << " already exists. Writing with new id..."
                << std::endl;
            return write(yaml, directory);
        }
    } catch (const std::exception& e) {
        log(ERROR) << "Could not read or create file " << file << "\n"
            << e.what() << std::endl;
        return false;
    }
    std::ofstream out(file);
    out << yaml.c_str();
    if (!out) {
        log(ERROR) << "Could not write bookmark to file: " << file << std::endl;
        return false;
    }
    return true;
}

/*Create x11 key event.*/
XKeyEvent create_key_event(
        Display* dpy,
        Window& win,
        Window& root,
        bool press,
        int keycode,
        int modifiers) {
    XKeyEvent evt;
    evt.display = dpy;
    evt.window = win;
    evt.root = root;
    evt.subwindow = None;
    evt.time = CurrentTime;
    evt.same_screen = True;
    evt.keycode = XKeysymToKeycode(dpy, keycode);
    evt.state = modifiers;
    evt.type = press ? KeyPress : KeyRelease;
    return evt;
}

} // namespace

bool fetch_url_and_title(std::string& url, std::string& title) {
    std::unique_ptr<xdo_t, decltype(&xdo_free)> xdo(xdo_new(nullptr), &xdo_free);

    Window win;
    xdo_get_active_window(xdo.get(), &win);
    unsigned char* win_name;
    int name_len;
    int win_type;
    xdo_get_window_name(xdo.get(), win, &win_name, &name_len, &win_type);

    constexpr const unsigned char chrome_win_suffix[17] = " - Google Chrome";
    constexpr const int suffix_len = sizeof(chrome_win_suffix)/sizeof(*chrome_win_suffix) - 1;
    if (std::memcmp(win_name + name_len - suffix_len, chrome_win_suffix, suffix_len)) {
        log(ERROR) << "Window \"" << win_name << "\" is not a Google Chrome window." << std::endl;
        return false;
    }

    /*focus the chrome address bar*/
    Display* dpy = XOpenDisplay(nullptr);
    Window root = XDefaultRootWindow(dpy);
    int revert;
    XGetInputFocus(dpy, &win, &revert);

    XEvent focus_evt;
    focus_evt.xkey = create_key_event(dpy, win, root, true, XK_l, ControlMask);
    XSendEvent(focus_evt.xkey.display, focus_evt.xkey.window, True, KeyPressMask, &focus_evt);

    XFlush(dpy);
    std::this_thread::sleep_for(std::chrono::milliseconds(3));

    /*copy the url*/
    XEvent copy_evt;
    copy_evt.xkey = create_key_event(dpy, win, root, true, XK_c, ControlMask);
    XSendEvent(copy_evt.xkey.display, copy_evt.xkey.window, True, KeyPressMask, &copy_evt);

    XFlush(dpy);
    XCloseDisplay(dpy);

    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    url = ::barn::x11::cp::get_text_from_clipboard();
    win_name[name_len - suffix_len] = '\0';
    title = std::string(reinterpret_cast<const char*>(win_name));

    return true;
}

void add_dialogs_to_app(x11::App& app) {
    for (const Dialog dialog : app.settings->dialog_sequence) {
        switch (dialog) {
            case Dialog::review_url:
                app.dialogs.emplace_back(std::make_shared<ReviewURLDialog>(app));
                break;
            case Dialog::ask_for_comment:
                app.dialogs.emplace_back(std::make_shared<AddCommentDialog>(app));
                break;
            case Dialog::ask_for_path:
                app.dialogs.emplace_back(std::make_shared<AddPathDialog>(app));
                break;
            case Dialog::ask_for_rating:
                app.dialogs.emplace_back(std::make_shared<AddRatingDialog>(app));
                break;
            case Dialog::ask_for_tags:
                app.dialogs.emplace_back(std::make_shared<AddTagsDialog>(app));
                break;
            default:
                log(WARN) << "Unknown dialog type: "
                    << static_cast<std::underlying_type<Dialog>::type>(dialog) << std::endl;
        }
    }
}

std::string remove_querystring(const std::string& url) {
    const size_t querystring_start_index = url.find_first_of('?');
    if (querystring_start_index != std::string::npos) {
        return url.substr(0, querystring_start_index);
    }
    return url;
}

bool save_bookmark(const Bookmark& bookmark, const fs::path& directory) {
    /*create bookmark folder*/
    try {
        fs::create_directories(directory);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not read or create directory " << directory << "\n"
            << e.what() << std::endl;
        return false;
    }

    /*create yaml*/
    YAML::Emitter yaml;
    yaml.SetIndent(4);
    yaml << YAML::BeginMap
        << YAML::Key << "title" << YAML::Value << bookmark.title
        << YAML::Key << "url" << YAML::Value << bookmark.url
        << YAML::Key << "created" << YAML::Value << bookmark.created.str()
        << YAML::Key << "last_access" << YAML::Value << bookmark.last_access.str()
        << YAML::Key << "rating" << YAML::Value << bookmark.rating
        << YAML::Key << "comment" << YAML::Value << YAML::Literal << bookmark.comment
        << YAML::Key << "tags" << YAML::Value << YAML::Flow << YAML::BeginSeq;
        for ( const std::string& tag : bookmark.tags) {
            yaml << tag;
        }
        yaml << YAML::EndSeq << YAML::EndMap;

    return write(yaml, directory);
}

ReviewURLDialog::ReviewURLDialog(x11::App& app) : x11::Dialog(app)
{}

void ReviewURLDialog::draw() {
    constexpr static const int win_width = 100;
    const std::string& url = app.context->bookmark.url;
    if (!is_initalized) {
        app.resize_window(14, win_width);
        url_without_querystring = remove_querystring(url);
        is_initalized = true;
    }

    const int title_length(app.context->bookmark.title.length());
    const int title_padding = std::max((win_width - title_length)/2, 1);

    app.draw_text(app.fc_text, app.context->bookmark.title, 1, title_padding);
    app.draw_text(app.fc_label, "Url:", 3, 2);

    if (url_without_querystring.size() != url.size()) {
        app.draw_text(app.fc_text, url_without_querystring, 4, 2);
        const std::string querystring = url.substr(url_without_querystring.size());
        const XGlyphInfo extents = app.get_text_extents(url_without_querystring);
        if (app.context->keep_querystring) {
            app.draw_text(app.fc_text, querystring, 4, 2, 0, extents.width);
            app.draw_text(app.fc_label, "(q) Delete Querystring", 7, 1);
        } else {
            app.draw_text(app.fc_hint, querystring, 4, 2, 0, extents.width);
            app.draw_text(app.fc_label, "(q) Keep Querystring", 7, 1);
        }
    } else {
        app.draw_text(app.fc_text, app.context->bookmark.url, 4, 2);
    }

    app.draw_text(app.fc_hint, "<Esc>: Cancel", 12, 1);
    app.draw_text(app.fc_hint, "<Tab>: Continue", 11, 83);
    app.draw_text(app.fc_hint, "<Ctrl> + <Enter>: Finish", 12, 74);
}

x11::AppState ReviewURLDialog::handle_key_press(XEvent& evt) {
    switch (evt.xkey.keycode) {
        case 9: /*esc*/
            app.context->do_store = false;
            break;
        case 24: /*q*/
            app.context->keep_querystring = !app.context->keep_querystring;
            app.redraw();
            break;
    }
    return Dialog::handle_key_press(evt);
}

AddPathDialog::AddPathDialog(x11::App& app)
:
x11::Dialog(app),
txt_path(app, 2, 2, 5, 0, 1, 94, 1)
{}

void AddPathDialog::draw() {
    constexpr static const int win_width = 100;
    if (!is_initalized) {
        app.resize_window(10, win_width);
        txt_path.set_focus(true);
        is_initalized = true;
    }

    app.draw_text(app.fc_label, "Directory:", 1, 2);
    txt_path.draw();

    app.draw_text(app.fc_hint, "<Esc>: Cancel", 8, 1);
    app.draw_text(app.fc_hint, "<Tab>: Continue", 7, 83);
    app.draw_text(app.fc_hint, "<Ctrl> + <Enter>: Finish", 8, 74);
}

x11::AppState AddPathDialog::handle_key_press(XEvent& evt) {
    switch(evt.xkey.keycode) {
        case 9: /*esc*/
            app.context->do_store = false;
            break;
        case 23: /*tab*/
            break;
        default:
            txt_path.handle_key_press(evt);
            app.redraw();
            app.context->bookmark_dir = txt_path.get_text();
            break;
    }
    return Dialog::handle_key_press(evt);
}

AddCommentDialog::AddCommentDialog(x11::App& app)
:
x11::Dialog(app),
txt_comment(app, 2, 2, 5, 0, 7, 94, 99)
{}

void AddCommentDialog::draw() {
    constexpr static const int win_width = 100;
    if (!is_initalized) {
        app.resize_window(14, win_width);
        txt_comment.set_focus(true);
        is_initalized = true;
    }

    app.draw_text(app.fc_label, "Comment:", 1, 2);
    txt_comment.draw();

    app.draw_text(app.fc_hint, "<Esc>: Cancel", 12, 1);
    app.draw_text(app.fc_hint, "<Tab>: Continue", 11, 83);
    app.draw_text(app.fc_hint, "<Ctrl> + <Enter>: Finish", 12, 74);
}

x11::AppState AddCommentDialog::handle_key_press(XEvent& evt) {
    switch(evt.xkey.keycode) {
        case 9: /*esc*/
            app.context->do_store = false;
            break;
        case 23: /*tab*/
            break;
        default:
            txt_comment.handle_key_press(evt);
            app.redraw();
            app.context->bookmark.comment = txt_comment.get_text();
            break;
    }
    return Dialog::handle_key_press(evt);
}

AddRatingDialog::AddRatingDialog(x11::App& app) : x11::Dialog(app)
{}

void AddRatingDialog::draw() {
    constexpr static const int win_width = 100;
    const std::string& url = app.context->bookmark.url;
    if (!is_initalized) {
        app.resize_window(10, win_width);
        is_initalized = true;
    }
}

AddTagsDialog::AddTagsDialog(x11::App& app) : x11::Dialog(app)
{}

void AddTagsDialog::draw() {
    constexpr static const int win_width = 100;
    const std::string& url = app.context->bookmark.url;
    if (!is_initalized) {
        app.resize_window(9, win_width);
        is_initalized = true;
    }
}

} // namespace bbm
} // namespace barn
