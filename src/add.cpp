/* Implementation for add.hpp.

author: andreasl
*/
#include "add.hpp"

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
    const std::string filename = generate_id() + ".yaml";
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
        Display* display,
        Window& win,
        Window& root,
        bool press,
        int keycode,
        int modifiers) {
    XKeyEvent evt;
    evt.display = display;
    evt.window = win;
    evt.root = root;
    evt.subwindow = None;
    evt.time = CurrentTime;
    evt.same_screen = True;
    evt.keycode = XKeysymToKeycode(display, keycode);
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
    if (std::memcmp(win_name + name_len - 16, chrome_win_suffix, 16)) {
        log(ERROR) << "Window \"" << win_name << "\" is not a Google Chrome window." << std::endl;
        return false;
    }

    /*focus the chrome address bar*/
    Display* display = XOpenDisplay(nullptr);
    Window root = XDefaultRootWindow(display);
    int revert;
    XGetInputFocus(display, &win, &revert);

    XEvent focus_evt;
    focus_evt.xkey = create_key_event(display, win, root, true, XK_l, ControlMask);
    XSendEvent(focus_evt.xkey.display, focus_evt.xkey.window, True, KeyPressMask, &focus_evt);

    XFlush(display);
    std::this_thread::sleep_for(std::chrono::milliseconds(3));

    /*copy the url*/
    XEvent copy_evt;
    copy_evt.xkey = create_key_event(display, win, root, true, XK_c, ControlMask);
    XSendEvent(copy_evt.xkey.display, copy_evt.xkey.window, True, KeyPressMask, &copy_evt);

    XFlush(display);
    XCloseDisplay(display);

    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    url = ::barn::x11::cp::get_text_from_clipboard();
    win_name[name_len - 16] = '\0';
    title = std::string(reinterpret_cast<const char*>(win_name));

    return true;
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

AddCommentDialog::AddCommentDialog(x11::App& app) : x11::Dialog(app)
{}

void AddCommentDialog::draw() {
    // TODO
}

x11::AppState AddCommentDialog::handle_key_press(XEvent& evt) {
    // TODO
    return x11::AppState::EXIT;
}

x11::AppState AddCommentDialog::handle_key_release(XEvent& evt) {
    // TODO
    return x11::AppState::EXIT;
}

AddPathDialog::AddPathDialog(x11::App& app) : x11::Dialog(app)
{}

void AddPathDialog::draw() {
    if (!is_initalized) {
        app.resize_window(14, 100);
        has_querystring = true; // TODO check if url has querystring
        is_initalized = true;
    }

    std::stringstream ss;
    ss << app.line_height;

    const auto title_padding = std::max((100 - app.context->bookmark.title.length())/2, 0ul);

    draw_text(app, app.fc_text, app.context->bookmark.title, 2, title_padding);
    draw_text(app, app.fc_label, "Url:", 4, 2);
    draw_text(app, app.fc_text, app.context->bookmark.url, 5, 2);

    if (has_querystring) {
        if(keep_querystring) {
            draw_text(app, app.fc_label, "(q) Delete Querystring", 8, 1);
        } else {
            draw_text(app, app.fc_label, "(q) Keep Querystring", 8, 1);
        }
    }

    draw_text(app, app.fc_hint, "<Esc>: Cancel", 13, 1);
    draw_text(app, app.fc_hint, "<Enter>: Continue", 12, 81);
    draw_text(app, app.fc_hint, "<Ctrl> + <Enter>: Finish", 13, 74);
}

x11::AppState AddPathDialog::handle_key_press(XEvent& evt) {
    if (evt.xkey.keycode == 9 /*esc*/) {
        return x11::AppState::EXIT;
    }
    return x11::AppState::KEEP_RUNNING;
}

x11::AppState AddPathDialog::handle_key_release(XEvent& evt) {
    // TODO
    return x11::AppState::KEEP_RUNNING;
}

AddRatingDialog::AddRatingDialog(x11::App& app) : x11::Dialog(app)
{}

void AddRatingDialog::draw() {
    // TODO
}

x11::AppState AddRatingDialog::handle_key_press(XEvent& evt) {
    // TODO
    return x11::AppState::EXIT;
}

x11::AppState AddRatingDialog::handle_key_release(XEvent& evt) {
    // TODO
    return x11::AppState::EXIT;
}

AddTagsDialog::AddTagsDialog(x11::App& app) : x11::Dialog(app)
{}

void AddTagsDialog::draw() {
    // TODO
}

x11::AppState AddTagsDialog::handle_key_press(XEvent& evt) {
    // TODO
}

x11::AppState AddTagsDialog::handle_key_release(XEvent& evt) {
    // TODO
}

} // namespace bbm
} // namespace barn
