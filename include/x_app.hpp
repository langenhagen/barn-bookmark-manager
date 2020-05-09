/* Common x11 application code.

author: andreasl
*/
#pragma once

#include <memory>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

namespace barn {
namespace bbm {

struct Context;
struct Settings;

namespace x11 {

struct App;

/*Transitional behaviors between dialog redraws.*/
enum class AppState {
    START  /*Start the first dialog.*/,
    EXIT  /*Exit the app.*/,
    KEEP_RUNNING  /*Stay in the current dialog*/,
    BACK  /*Go back to the last dialog.*/,
    PROCEED /*Proceed to the next dialog.*/
};

/*Represents dialogs.*/
struct Dialog {
    App& app;  /*Enclosing application.*/

    Dialog(App& app);  /*Constructor.*/
    virtual void draw() = 0;  /*Draw the window.*/
    virtual AppState handle_key_press(XEvent& evt) = 0;  /*Handle key press events.*/
    virtual AppState handle_key_release(XEvent& evt) = 0;  /*Handle key release events.*/
};

/*Single-window x11 application for dialog sequences.*/
struct App {
    const std::shared_ptr<Settings> settings;  /*Application settings.*/
    std::shared_ptr<Context> context;  /*The application's data.*/

    Display* display;  /*X11 display.*/
    int screen;  /*X11 screen number.*/
    Window root_win;  /*X11 root window.*/
    Window win;  /*X11 application window.*/
    GC gc;  /*X11 graphics context.*/
    XftDraw* xft_drawable;  /*Xft text drawable.*/
    XftFont* font;  /*Xft text font.*/
    constexpr static const float font_size = 10.0;  /*Font size.*/
    unsigned int line_height;  /*Font-dependent line height.*/

    const int app_bg;  /*Application background color.*/
    XftColor fc_text;  /*Text color.*/
    XftColor fc_label;  /*Explanatory text color.*/
    XftColor fc_hint;  /*Hint text color.*/

    bool ctrl_l = false;  /*Indicates whether left control is pressed.*/
    bool ctrl_r = false;  /*Indicates whether right control is pressed.*/
    bool shift_l = false;  /*Indicates whether left shift is pressed.*/
    bool shift_r = false;  /*Indicates whether right shift is pressed.*/

    std::vector<std::shared_ptr<Dialog>> dialogs;  /*Application Dialog list.*/
    std::vector<std::shared_ptr<Dialog>>::iterator dialog_it;  /*Current dialog.*/

    /*Constructor.*/
    App(const std::shared_ptr<Settings>& settings, std::shared_ptr<Context>& context);
    ~App();  /*Destructor.*/

    bool is_ctrl_pressed() const;  /*Indicate if a ctrl-button is pressed.*/
    bool is_shift_pressed() const;  /*Indicate if a shift-button is pressed.*/

    Window setup_window();  /*Create an x11 window.*/
    bool grab_keyboard();  /*Grab keyboard focus.*/
    int resize_window(int rows, int cols);  /*Resize the application window.*/
    void redraw();  /*Clean redraw the window.*/
    AppState handle_key_press(XEvent& evt);  /*Handle key press events.*/
    AppState handle_key_release(XEvent& evt);  /*Handle key release events.*/
    void run();  /*Start and run the application.*/
};

/*Allocate an Xft color.*/
XftColor alloc_color(Display* dpy, const int screen, const XRenderColor& color);

/*Free a given Xft color.*/
void free_color(Display* dpy, const int screen, XftColor& xft_color);

/*Draw text*/
void draw_text(
        const x11::App& app,
        const XftColor& xft_color,
        const std::string& str,
        const float row,
        const float col);

} // namespace x11
} // namespace bbm
} // namespace barn
