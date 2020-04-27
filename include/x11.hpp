/* Common x11 gui code.

author: andreasl
*/
#pragma once

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

namespace barn {
namespace bbm {
namespace ui {

/*Simple x11 window application for for text I/O.*/
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

    /*Draw the text.*/
    void draw_text();

    /*Clear the view and redraw the app's elements.*/
    void redraw();

    /*Handle key press events.*/
    int handle_key_press(XEvent& evt);

    /*Handle key release events.*/
    int handle_key_release(XEvent& evt);

    /*Run the application loop and exit with an error code.*/
    int run();
};

} // namespace ui
} // namespace bbm
} // namespace barn