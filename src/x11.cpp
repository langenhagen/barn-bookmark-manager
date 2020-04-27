/* Implementation for x11.hpp.

author: andreasl
*/
#include "x11.hpp"

#include "log.hpp"

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include <chrono>
#include <string>
#include <thread>

#include <iostream>

namespace barn {
namespace bbm {
namespace ui {

namespace {

/*UI font size.*/
constexpr const float font_size = 16.0;

} // namespace

/*Constructor.*/
App::App() {
    this->display = XOpenDisplay(nullptr);
    this->screen = DefaultScreen(this->display);
    this->root_win = RootWindow(this->display, this->screen);

    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    attrs.background_pixel = 0x222222;
    attrs.event_mask =
        ExposureMask
        | KeyPressMask
        | KeyReleaseMask
        | VisibilityChangeMask;

    this->win = XCreateWindow(
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

    std::string win_name = "Barn's Bookmark Manager";
    XSetStandardProperties(
        this->display,
        this->win,
        win_name.c_str(),
        "",
        None,
        nullptr,
        0,
        nullptr);

    setup_xft_font();
    resize_window(1, win_name.length());

    this->gc = XCreateGC(this->display, this->win, 0, 0);
    XClearWindow(this->display, this->win);
    XMapRaised(this->display, this->win);

    grab_keyboard();
}

/*Desctructor.*/
App::~App() {
    XFreeGC(this->display, this->gc);
    XDestroyWindow(this->display, this->win);
    XCloseDisplay(this->display);
}

/*Specify and load the xft font.*/
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
        font_size,
        nullptr);

    this->line_height = this->font->ascent + this->font->descent;
}

/*Attempt to grab the keyboard.*/
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

/*Resize the window.*/
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

/*Draw text.*/
void App::draw_text() {
    XRenderColor x_color = {65535, 65535, 65535, 65535};

    XftColor xft_color;
    XftColorAllocValue(
        this->display,
        DefaultVisual(this->display, this->screen),
        DefaultColormap(this->display, this->screen),
        &x_color,
        &xft_color);

        std::string text = "Barn's Bookmark Manager";
        XftDrawString8(
            this->xft_drawable,
            &xft_color,
            this->font,
            0,
            this->font->ascent,
            (unsigned char*)text.c_str(),
            text.length());
    XftColorFree(
        this->display,
        DefaultVisual(this->display, 0),
        DefaultColormap(this->display, 0),
        &xft_color);
}

/*Clear the view and redraw the app's elements.*/
void App::redraw() {
    XClearWindow(this->display, this->win);
    draw_text();
}

/*Handle key press events.*/
int App::handle_key_press(XEvent& evt) {
    log(INFO) << "handle_key_press" << std::endl;
    return 1;
}

/*Handle key release events.*/
int App::handle_key_release(XEvent& evt) {
    log(INFO) << "handle_key_release" << std::endl;
    return 0;
}

/*Run the application loop and exit with an error code.*/
int App::run() {
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

} // namespace ui
} // namespace bbm
} // namespace barn
