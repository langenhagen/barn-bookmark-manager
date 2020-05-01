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
namespace x11 {

struct App;

/*Represents dialogs.*/
struct Dialog {
    App& app;  /*Surrounding application.*/

    Dialog(App& app);  /*Constructor.*/

    virtual void draw() = 0;  /*Draw the window.*/
    virtual int handle_key_press(XEvent& evt) = 0;  /*Handle key press events.*/
    virtual int handle_key_release(XEvent& evt) = 0;  /*Handle key release events.*/
};

/*Simple single-window x11 application for dialog sequences.*/
struct App {
    using DialogVector = std::vector<std::shared_ptr<Dialog>>;
    using DialogVectorIter = DialogVector::iterator::iterator_type;

    Display* display;  /*X11 display.*/
    int screen;  /*X11 screen number.*/
    Window root_win;  /*X11 root window.*/
    Window win;  /*X11 application window.*/
    GC gc;  /*X11 graphics context*/
    XftDraw* xft_drawable;  /*Xft text drawable.*/
    XftFont* font;   /*Xft text font.*/
    constexpr static const float font_size = 16.0;  /*Font size.*/
    unsigned int line_height;  /*Font-dependent line height*/
    DialogVector dialogs;  /*Application Dialog list*/
    DialogVectorIter dialog_it;  /*Current application dialog.*/

    bool is_ctrl_pressed = false;  /*Specifies if a ctrl button is pressed.*/

    App();  /*Constructor.*/
    ~App();  /*Destructor.*/

    Window setup_window();  /*Create an x11 window.*/
    void setup_xft_font();  /*Setup Xft and font related variables.*/
    int grab_keyboard();  /*Grab keyboard focus.*/
    void resize_window(int rows, int cols);  /*Resize the application window.*/
    void redraw();  /*Clean redraw the window.*/
    int handle_key_press(XEvent& evt);  /*Handle key press events.*/
    int handle_key_release(XEvent& evt);  /*Handle key release events.*/
    int run();  /*.Start and run the application.*/
};

} // namespace x11
} // namespace bbm
} // namespace barn
