/* Implementation for x_app.hpp.

author: andreasl
*/
#include "x_app.hpp"

#include "bookmark.hpp"
#include "log.hpp"

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace barn {
namespace bbm {

struct Context;
struct Settings;

namespace x11 {

Dialog::Dialog(App& application) : app(application)
{}

App::App(const std::shared_ptr<Settings>& settings, std::shared_ptr<Context>& context)
:
settings(settings),
context(context),
display(XOpenDisplay(nullptr)),
screen(DefaultScreen(display)),
root_win(RootWindow(display, screen)),
win(setup_window()),
gc(XCreateGC(display, win, 0, 0)),
xft_drawable(XftDrawCreate(display, win, DefaultVisual(display, 0), DefaultColormap(display, 0))),
font(XftFontOpen(
    display,
    screen,
    XFT_FAMILY,
    XftTypeString,
    "monospace",
    XFT_SIZE,
    XftTypeDouble,
    font_size,
    nullptr)),
line_height(font->ascent + font->descent) {

    XSetStandardProperties(
        this->display,
        this->win,
        "Barn's Bookmark Manager",
        "",
        None,
        nullptr,
        0,
        nullptr);
}

App::~App() {
    XFreeGC(this->display, this->gc);
    XDestroyWindow(this->display, this->win);
    XCloseDisplay(this->display);
}

Window App::setup_window() {
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    attrs.background_pixel = 0x222222;
    attrs.event_mask =
        ExposureMask
        | KeyPressMask
        | KeyReleaseMask
        | VisibilityChangeMask;

    return XCreateWindow(
        this->display,
        this->root_win,
        1,
        1,
        1,
        1,
        0,
        CopyFromParent,
        CopyFromParent,
        CopyFromParent,
        CWOverrideRedirect | CWBackPixel | CWEventMask,
        &attrs);
}

bool App::grab_keyboard() {
    /*try to grab keyboard 1000 times.
    We may have to wait for another process to ungrab.*/
    for (int i = 0; i < 1000; ++i) {
        const int grab_result = XGrabKeyboard(
            this->display,
            this->root_win,
            True,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime);
        if (grab_result == GrabSuccess) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    log(ERROR) << "Could not grab keyboard" << std::endl;
    return false;
}

int App::resize_window(int rows, int cols) {
    const Screen* const screen DefaultScreenOfDisplay(this->display);
    const auto width = this->font->max_advance_width * cols;
    const auto height = this->line_height * rows;
    return XMoveResizeWindow(
        this->display,
        this->win,
        (screen->width - width) / 2,
        (screen->height - height) / 3,
        width,
        height);
}

void App::redraw() {
    XClearWindow(this->display, this->win);
    dialog_it->draw();
}

bool App::is_ctrl_pressed() const {
    return this->ctrl_l || this->ctrl_r;
}
bool App::is_shift_pressed() const {
    return this->shift_l || this->shift_r;
}

AppState App::handle_key_press(XEvent& evt) {
    switch(evt.xkey.keycode) {
        case 37: /*ctrl left*/
            this->ctrl_l = true;
            break;
        case 105: /*ctrl right*/
            this->ctrl_r = true;
            break;
        case 50: /*shift left*/
            this->shift_l = true;
            break;
        case 62: /*shift right*/
            this->shift_r = true;
            break;
    }
    return dialog_it->handle_key_press(evt);
}

AppState App::handle_key_release(XEvent& evt) {
    switch(evt.xkey.keycode) {
        case 37: /*ctrl left*/
            this->ctrl_l = false;
            break;
        case 105: /*ctrl right*/
            this->ctrl_r = false;
            break;
        case 50: /*shift left*/
            this->shift_l = false;
            break;
        case 62: /*shift right*/
            this->shift_r = false;
            break;
    }
    return dialog_it->handle_key_release(evt);
}

void App::run() {
    if (!grab_keyboard()) {
        return;
    } else if (dialog_it == dialogs.end()) {
        log(WARN) << "Past last dialog." << std::endl;
        return;
    }
    XMapRaised(this->display, this->win);

    XEvent evt;
    AppState state = AppState::KEEP_RUNNING;
    while (state != AppState::EXIT && !XNextEvent(this->display, &evt)) {
        switch (evt.type) {
            case Expose:
                if (evt.xexpose.count == 0) {
                    redraw();
                }
                break;
            case VisibilityNotify:
                if (evt.xvisibility.state != VisibilityUnobscured) {
                    XRaiseWindow(this->display, this->win);
                }
                break;
            case KeyPress:
                state = handle_key_press(evt);
                break;
            case KeyRelease:
                state = handle_key_release(evt);
                break;
        }
        switch (state) {
            case AppState::PROCEED:
                ++dialog_it;
                if (dialog_it == dialogs.end()) {
                    state = AppState::EXIT;
                }
                break;
            case AppState::BACK:
                if (dialog_it != dialogs.begin()) {
                    --dialog_it;
                }
                break;
        }
    }
}

} // namespace x11
} // namespace bbm
} // namespace barn
