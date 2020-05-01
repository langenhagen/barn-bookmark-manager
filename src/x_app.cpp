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
namespace x11 {

Dialog::Dialog(App& application) : app(application)
{}

App::App() {
    this->display = XOpenDisplay(nullptr);
    this->screen = DefaultScreen(this->display);
    this->root_win = RootWindow(this->display, this->screen);
    this->win = setup_window();
    this->gc = XCreateGC(this->display, this->win, 0, 0);

    XSetStandardProperties(
        this->display,
        this->win,
        "Barn's Bookmark Manager",
        "",
        None,
        nullptr,
        0,
        nullptr);

    setup_xft_font();
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

void App::setup_xft_font() {
    this->xft_drawable = XftDrawCreate(
        this->display,
        this->win,
        DefaultVisual(this->display, 0),
        DefaultColormap(this->display, 0));

    this->font = XftFontOpen(
        this->display,
        this->screen,
        XFT_FAMILY,
        XftTypeString,
        "monospace",
        XFT_SIZE,
        XftTypeDouble,
        this->font_size,
        nullptr);

    this->line_height = this->font->ascent + this->font->descent;
}

int App::grab_keyboard() {
    /*try to grab keyboard 1000 times.
    We may have to wait for another process to ungrab.*/
    for(int i = 0; i < 1000; ++i) {
        const int grab_result = XGrabKeyboard(
            this->display,
            this->root_win,
            True,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime);
        if(grab_result == GrabSuccess) {
            return 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    log(ERROR) << "Could not grab keyboard" << std::endl;
    return 1;
}

void App::resize_window(int rows, int cols) {
    const Screen* const screen DefaultScreenOfDisplay(this->display);
    const auto width = this->font->max_advance_width * cols;
    const auto height = this->line_height * rows;
    XMoveResizeWindow(
        this->display,
        this->win,
        (screen->width - width) / 2,
        (screen->height - height) / 3,
        width,
        height);
}

void App::redraw() {
    XClearWindow(this->display, this->win);
}

int App::handle_key_press(XEvent& evt) {
    log(INFO) << "handle_key_press" << std::endl;
    return 1;
}

int App::handle_key_release(XEvent& evt) {
    log(INFO) << "handle_key_release" << std::endl;
    return 1;
}

int App::run() {
    XMapRaised(this->display, this->win);

    // TODO show the dialogs

    XEvent evt;
    while(!XNextEvent(this->display, &evt)) {
        switch(evt.type) {
        case Expose:
            if(evt.xexpose.count == 0) {
                redraw();
            }
            break;
        case VisibilityNotify:
            if (evt.xvisibility.state != VisibilityUnobscured) {
                XRaiseWindow(this->display, this->win);
            }
            break;
        case KeyPress:
            if(handle_key_press(evt)) {
                return 0;
            }
            break;
        case KeyRelease:
            if(handle_key_release(evt)) {
                return 0;
            }
            break;
        }
    }
}

} // namespace x11
} // namespace bbm
} // namespace barn
