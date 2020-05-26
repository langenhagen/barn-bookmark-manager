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
    bool is_initalized = false;  /*Indicate whether the dialog has been set up.*/

    Dialog(App& app);  /*Constructor.*/
    virtual void draw() = 0;  /*Draw the window.*/
    virtual AppState handle_key_press(XEvent& evt);  /*Handle key press events.*/
    virtual AppState handle_key_release(XEvent& evt);  /*Handle key release events.*/
};

/*Single-window x11 application for dialog sequences.*/
struct App {
    const std::shared_ptr<Settings> settings;  /*Application settings.*/
    std::shared_ptr<Context> context;  /*The application's data.*/

    Display* dpy;  /*X11 dpy.*/
    int screen;  /*X11 screen number.*/
    Window root_win;  /*X11 root window.*/
    Window win;  /*X11 application window.*/
    GC gc;  /*X11 graphics context.*/
    XftDraw* xft_drawable;  /*Xft text drawable.*/
    XftFont* font;  /*Xft text font.*/
    unsigned int win_width;  /*Window width.*/
    unsigned int win_height;  /*Window height.*/
    constexpr static const float font_size = 10.0;  /*Font size.*/
    unsigned int line_height;  /*Font-dependent line height.*/

    const int bc_app;  /*Application background color.*/
    const int fc_app_frame;  /*Application frame color.*/
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

    Window setup_window();  /*Create an x11 window.*/
    bool grab_keyboard();  /*Grab keyboard focus.*/
    int resize_window(int rows, int cols);  /*Resize the application window.*/
    XGlyphInfo get_text_extents(const std::string& str);  /*Get the given text dimensions.*/
    void draw_rect_frame();  /*Draw a small frame at the window edges*/
    /*Draw text at the given row/column.*/
    void draw_text(
        const XftColor& color,
        const std::string& str,
        const float row,
        const float col,
        const int y_px_offset = 0,
        const int x_px_offset = 0);
    void redraw();  /*Clean redraw the window.*/

    bool is_ctrl_pressed() const;  /*Indicate if a ctrl-button is pressed.*/
    bool is_shift_pressed() const;  /*Indicate if a shift-button is pressed.*/
    AppState handle_key_press(XEvent& evt);  /*Handle key press events.*/
    AppState handle_key_release(XEvent& evt);  /*Handle key release events.*/

    void run();  /*Run the application.*/

    XftColor alloc_color(const XRenderColor& color);  /*Allocate an Xft color.*/
    void free_color(XftColor& xft_color);  /*Free a given Xft color.*/
};

} // namespace x11
} // namespace bbm
} // namespace barn
