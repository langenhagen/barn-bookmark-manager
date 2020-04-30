/* Common x11 gui code.

author: andreasl
*/
#pragma once

#include <memory>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

namespace barn {
namespace bbm {

class Bookmark;

namespace x11 {

/*Represents dialogs.*/
struct Dialog {

    /*the bookmark*/
    std::shared_ptr<Bookmark> bookmark;

    Dialog(std::shared_ptr<Bookmark> bm);

    /*Draw the dialog.*/
    virtual void draw() = 0;

    /*Handle key press events.*/
    virtual int handle_key_press(XEvent& evt) = 0;

    /*Handle key release events.*/
    virtual int handle_key_release(XEvent& evt) = 0;
};

/*Simple single-window x11 application for dialog sequences.*/
struct App {
    /*x11 essentials*/
    Display* display;
    int screen;
    Window root_win;
    Window win;
    GC gc;

    /*Xft stuff*/
    XftDraw* xft_drawable;
    XftFont* font;

    /*application stuff*/
    using DialogVector = std::vector<std::shared_ptr<Dialog>>;
    DialogVector dialogs;
    typeof(DialogVector::iterator) current_dialog;
    constexpr static const float font_size = 16.0;
    unsigned int line_height;

    /*input stuff*/
    bool is_ctrl_pressed = false;

    /*constructors & destructor*/

    /*Constructor.*/
    App();

    /*Desctructor.*/
    ~App();

    /*methods*/

    /*Specify and load the xft font.*/
    void setup_xft_font();

    /*Attempt to grab the keyboard.*/
    int grab_keyboard();

    /*Resize the window.*/
    void resize_window(int rows, int cols);

    /*Clear the view and redraw the app's elements.*/
    void redraw();

    /*Handle key press events.*/
    int handle_key_press(XEvent& evt);

    /*Handle key release events.*/
    int handle_key_release(XEvent& evt);

    /*Run the application loop and exit with an error code.*/
    int run();
};

} // namespace x11
} // namespace bbm
} // namespace barn