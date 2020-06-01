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
    int line_height;  /*Font-dependent line height.*/

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

/*Representation of a text line.*/
struct Line {
    size_t buf_size;  /*Raw text buffer size.*/
    char* buf;  /*Text buffer.*/
    unsigned int len = 0;  /*Length of the string in the line.*/

    explicit Line(const size_t buf_size = 256);  /*Constructor.*/
    explicit Line(const Line& other);  /*Copy constructor.*/
    Line(Line&& other);  /*Move constructor.*/
    Line& operator=(Line&& other);  /*Move assignment operator.*/
    ~Line();  /*Destructor.*/
};

/*Representation of integer coordinates.*/
struct IntCoord {
    int y;  /*Row.*/
    int x;  /*Column.*/
};

/*Representation of text coordinates.*/
using TextCoord = IntCoord;

bool operator==(const TextCoord& lhs, const TextCoord& rhs);  /*Test 2 TextCoords for equality.*/

/*Representation of a text input widget.*/
class TextBox {
private:  /*constants*/
    constexpr static const int _cur_width = 3;  /*Width of the cursor.*/
    constexpr static const int _padding = 5;  /*Inner padding.*/
    constexpr static const XRenderColor _fc_text{65535, 65535, 65535, 65535};  /*Text color.*/
    constexpr static const unsigned long _bc_widget = 0x222222;  /*Background color.*/
    constexpr static const unsigned long _fc_cursor = 0xffffff;  /*Cursor color.*/
    constexpr static const unsigned long _bc_selection = 0x444477;  /*Selection back color.*/
    constexpr static const unsigned long _fc_border = 0xaaaaaa;  /*Border color.*/
    constexpr static const unsigned long _fc_border_inactive = 0x666666;  /*Inactive border color.*/

public:  /*vars*/
    const unsigned int row;  /*Widget y position in rows.*/
    const unsigned int col;  /*Widget x position in cols.*/
    const int y_px_offset;  /*Widget y offset in pixels.*/
    const int x_px_offset;  /*Widget x offset in pixels.*/
    const unsigned int height; /*Widget height in cols.*/
    const unsigned int width;  /*Widget width in rows.*/
    const unsigned int max_n_lines;  /*The maximum number of lines that are allowed for input.*/

private:  /*vars*/
    App& _app;  /*Enclosing application.*/
    std::vector<Line> _lines;  /*The lines containing the text.*/
    TextCoord _cur = {0, 0};  /*Cursor position.*/
    TextCoord _sel_start = {-1, -1};  /*Selection boundary.*/
    IntCoord _off = {0, 0};  /*View offset.*/
    bool _has_focus = false;  /*Specifies whether the widget should have the focus.*/

public:  /*methods*/
    /*Constructor.*/
    TextBox(
        App& app,
        const unsigned int row,
        const unsigned int col,
        const int y_px_offset,
        const int x_px_offset,
        const unsigned int height,
        const unsigned int width,
        const unsigned int max_n_lines = 0);

    bool has_focus() const;  /*Check whether the widget has focus.*/
    void set_focus(const bool has_focus);  /*Specify whether the widget has focus.*/

    void draw();  /*Draw the widget.*/
    std::string get_text();  /*Get the text from the widget.*/

    void move_cursor(int inc);  /*Move the cursor by increment forward/backward.*/
    void move_cursor_vertically(const int inc);  /*Move the cursor by inc up/down*/
    void move_cursor_by_word(int n_words);  /*Move the cursor by n words.*/

    void insert_char(const char c);  /*Insert given character at the current cursor position.*/
    void delete_chars(int n_chars);  /*Delete given number characters at the cursor position.*/
    bool delete_selected_text();  /*Delete the text within the current selection.*/

    int handle_key_press(XEvent& evt);  /*Handle key press events.*/
    int handle_key_release(XEvent& evt);  /*Handle key release events.*/

private: /*methods*/
    unsigned int _y() const;  /*Get the y ordinate in pixels.*/
    unsigned int _x() const;  /*Get the x abscissa in pixels.*/
    unsigned int _height() const;  /*Get the x abscissa in pixels.*/
    unsigned int _width() const;  /*Get the y ordinate in pixels.*/

    void _start_selection();  /*Set the variable selection_start to the current cursor position.*/
    void _invalidate_selection();  /*Set selection-related member variables to invalid values.*/
    /*Get the TextCoords of beginning and end of the current selection.*/
    std::pair<const TextCoord&, const TextCoord&> _get_selection_bounds() const;
    std::string _get_selected_text() const;  /*Get the currently selected text as a string.*/
    void _write_selected_text_to_clipboard() const;  /*Write the selected text to clipboard.*/
    IntCoord _calc_cursor_pos() const;  /*Calculate the cursor position in pixels.*/
    void _adjust_offset(const IntCoord& coords);  /*Update the view offset to contain the cursor.*/

    void _draw_background();  /*Draw the background and the border.*/
    void _draw_text();  /*Draw the text.*/
    void _draw_cursor(const IntCoord& coords);  /*Draw the text cursor.*/
    void _draw_selection();  /*Draw the selection rectangles.*/

    bool _insert_text(const char* str);  /*Insert given string at the current cursor position.*/
    bool _insert_newline();  /*Insert a newline.*/
    /*Delete the text between given TextCoords.*/
    void _delete_text(const TextCoord& start, const TextCoord& end);
};

} // namespace x11
} // namespace bbm
} // namespace barn
