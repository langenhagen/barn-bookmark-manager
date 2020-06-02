/* Implementation for x_app.hpp.

author: andreasl
*/
#include "x_app.hpp"

#include "bookmark.hpp"
#include "log.hpp"
#include "x_copy_paste.hpp"

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include <chrono>
#include <cstring>
#include <memory>
#include <string>
#include <thread>

namespace barn {
namespace bbm {

struct Context;
struct Settings;

namespace x11 {

Line::Line(const size_t buf_size) : buf_size(buf_size), buf(new char[buf_size])
{}

Line::Line(const Line& other) : buf_size(other.buf_size), buf(new char[buf_size]), len(other.len) {
    std::memcpy(this->buf, other.buf, this->buf_size);
}

Line::Line(Line&& other)
:
buf_size(std::move(other.buf_size)),
buf(new char[buf_size]),
len(std::move(other.len))
{
    std::memcpy(this->buf, other.buf, this->buf_size);
}

Line& Line::operator=(Line&& other) {
    this->buf_size = std::move(other.buf_size);
    delete[] this->buf;
    this->buf = new char[this->buf_size];
    std::memcpy(this->buf, other.buf, this->buf_size);
    this->len = std::move(other.len);
    return *this;
}

Line::~Line() {
    delete[] this->buf;
}

bool operator==(const TextCoord& lhs, const TextCoord& rhs) {
    return lhs.y == rhs.y && lhs.x == rhs.x;
}

TextBox::TextBox(
    App& app,
    const unsigned int row,
    const unsigned int col,
    const int y_px_offset,
    const int x_px_offset,
    const unsigned int height,
    const unsigned int width,
    const unsigned int max_n_lines)
:
row(row),
col(col),
y_px_offset(y_px_offset),
x_px_offset(x_px_offset),
height(height),
width(width),
max_n_lines(max_n_lines),
_app(app)
{
    _lines.emplace_back();
}

unsigned int TextBox::_y() const {
    return this->row * _app.line_height + this->y_px_offset;
}

unsigned int TextBox::_x() const {
    return this->col * _app.font->max_advance_width + this->x_px_offset;
}

unsigned int TextBox::_width() const {
    return this->width * _app.font->max_advance_width + 2 * _padding;
}

unsigned int TextBox::_height() const {
    return this->height * _app.line_height +  2 * _padding;
}

void TextBox::_start_selection() {
    if (_sel_start.y == -1) {
        _sel_start.y = _cur.y;
        _sel_start.x = _cur.x;
    }
}



void TextBox::_invalidate_selection() {
    if (_sel_start.y != -1) {
        _sel_start.y = -1;
        _sel_start.x = -1;
    }
}

std::pair<const TextCoord&, const TextCoord&> TextBox::_get_selection_bounds() const {
    const TextCoord& sel_start = _sel_start;
    if (_cur.y < sel_start.y || (_cur.y == sel_start.y && _cur.x < sel_start.x)) {
        return std::pair<const TextCoord&, const TextCoord&>(_cur, sel_start);
    }
    return std::pair<const TextCoord&, const TextCoord&>(sel_start, _cur);
}

std::string TextBox::_get_selected_text() const {
    if (_sel_start.y == -1) {
        return "";
    }

    const auto sel = _get_selection_bounds();
    int str_len = sel.second.x - sel.first.x + 1;
    for (auto i = sel.first.y; i < sel.second.y; ++i) {
        str_len += _lines[i].len + 1;
    }
    char str[str_len];
    if (sel.first.y == sel.second.y) {
        std::memcpy(
            str,
            &_lines[sel.first.y].buf[sel.first.x],
            str_len - 1);
    } else {
        int off = 0;
        for (auto i = sel.first.y; i <= sel.second.y; ++i) {
            const auto& line = _lines[i];
            int col = 0;
            int len = line.len;
            if (i == sel.first.y) {
                col = sel.first.x;
                len -= sel.first.x;
            }
            if (i == sel.second.y) {
                len = sel.second.x;
            }
            std::memcpy(&str[off], &line.buf[col], len);
            off +=len;
            if (i != sel.second.y) {
                str[off++] = '\n';
            }
        }
    }
    str[str_len - 1] = '\0';
    return std::string(str);
}

void TextBox::_write_selected_text_to_clipboard() const {
    ::barn::x11::cp::write_to_clipboard(_get_selected_text());
}

std::string TextBox::get_text() {
    size_t len = 0;
    for (const auto& line : _lines) {
        len += line.len + 1;
    }
    --len;

    std::string str(len, ' ');
    size_t index = 0;
    for (const auto& line : _lines) {
        for (size_t i = 0; i < line.len; ++i) {
            str[index++] = line.buf[i];
        }
        str[index++] = '\n';
    }
    return str;
}

IntCoord TextBox::_calc_cursor_pos() const {
    const auto& line = _lines[_cur.y];
    XGlyphInfo glyph_info_all;
    XftTextExtents8(
        _app.dpy,
        _app.font,
        reinterpret_cast<const XftChar8*>(line.buf),
        line.len,
        &glyph_info_all);
    XGlyphInfo glyph_info_remaining;
    XftTextExtents8(
        _app.dpy,
        _app.font,
        reinterpret_cast<const XftChar8*>(&line.buf[_cur.x]),
        line.len - _cur.x,
        &glyph_info_remaining);

    return IntCoord{
        _app.line_height * _cur.y + _padding,
        glyph_info_all.width - glyph_info_remaining.width + _padding};
}

void TextBox::_adjust_offset(const IntCoord& cur_coords) {
    if (cur_coords.y - _off.y < _padding) {
        /*cursor above visible area*/
        const int increment = (_padding - cur_coords.y + _off.y) / _app.line_height;
        _off.y -= increment * _app.line_height;
    } else if (cur_coords.y + _app.line_height - _off.y > this->_height() - _padding) {
        /*cursor below visible area*/
        const int increment =
            (cur_coords.y + _app.line_height - _off.y - this->_height() + _padding) /
            _app.line_height;
        _off.y += (increment + 1) * _app.line_height;
    }
    if (cur_coords.x - _off.x < _padding) {
        /*cursor left of the visible area*/
        const int increment = _padding - cur_coords.x + _off.x;
        _off.x -= increment;
    } else if (cur_coords.x + _cur_width - _off.x > this->_width() - _padding) {
        /*cursor right of the visible area*/
        const int increment = cur_coords.x + _cur_width - _off.x - this->_width() + _padding;
        _off.x += increment;
    }
}

void TextBox::_draw_background() {
    XSetForeground(_app.dpy, _app.gc, _bc_widget);
    XFillRectangle(
        _app.dpy,
        _app.win,
        _app.gc,
        this->_x(),
        this->_y(),
        this->_width(),
        this->_height());

    const unsigned long fc_border = _has_focus ? _fc_border : _fc_border_inactive;
    XSetForeground(_app.dpy, _app.gc, fc_border);
    XDrawRectangle(
        _app.dpy,
        _app.win,
        _app.gc,
        this->_x(),
        this->_y(),
        this->_width(),
        this->_height());
}

void TextBox::_draw_text() {
    XftColor xft_color;
    XftColorAllocValue(
        _app.dpy,
        DefaultVisual(_app.dpy, _app.screen),
        DefaultColormap(_app.dpy, _app.screen),
        &_fc_text,
        &xft_color);

    for (size_t i = 0; i < _lines.size(); ++i) {
        auto& line = _lines[i];
        XftDrawString8(
            _app.xft_drawable,
            &xft_color,
            _app.font,
            this->_x() + _padding - _off.x,
            _app.line_height * i + _app.font->ascent + this->_y() + _padding - _off.y,
            reinterpret_cast<unsigned char*>(line.buf),
            line.len);
    }
    XftColorFree(
        _app.dpy,
        DefaultVisual(_app.dpy, 0),
        DefaultColormap(_app.dpy, 0),
        &xft_color);
}

void TextBox::_draw_cursor(const IntCoord& coords) {
    XSetForeground(_app.dpy, _app.gc, _fc_cursor);
    XFillRectangle(
        _app.dpy,
        _app.win,
        _app.gc,
        coords.x + this->_x() - _off.x,
        coords.y + this->_y() - _off.y,
        _cur_width,
        _app.line_height);
}

void TextBox::_draw_selection() {
    if (_sel_start.y == -1 || _sel_start == _cur) {
        return;
    }

    const auto sel = _get_selection_bounds();
    for (auto i = sel.first.y; i <= sel.second.y; ++i) {
        const auto& line = _lines[i];
        int x = 0;

        XGlyphInfo glyph_info_all;
        XftTextExtents8(
            _app.dpy,
            _app.font,
            reinterpret_cast<const XftChar8*>(line.buf),
            line.len,
            &glyph_info_all);

        int width = glyph_info_all.width;
        if (i == sel.first.y) {
            XGlyphInfo glyph_info_selection;
            XftTextExtents8(
                _app.dpy,
                _app.font,
                reinterpret_cast<const XftChar8*>(&line.buf[sel.first.x]),
                line.len - sel.first.x,
                &glyph_info_selection);

            x = glyph_info_all.width - glyph_info_selection.width;
            width = glyph_info_selection.width;
        }

        if (i == sel.second.y) {
            XGlyphInfo glyph_info_remaining;
            XftTextExtents8(
                _app.dpy,
                _app.font,
                reinterpret_cast<const XftChar8*>(&line.buf[sel.second.x]),
                line.len - sel.second.x,
                &glyph_info_remaining);
             width -= glyph_info_remaining.width;
        }

        XSetForeground(_app.dpy, _app.gc, _bc_selection);
        XFillRectangle(
            _app.dpy,
            _app.win,
            _app.gc,
            x + this->_x() + _padding - _off.x,
            _app.line_height * i + this->_y() + _padding - _off.y,
            width,
            _app.line_height);
    }
}

void TextBox::move_cursor(int inc) {
    /*Move the cursor by increment to the left/right and consider line- and text- starts & ends.*/
    if (!_app.is_shift_pressed()) {
        _invalidate_selection();
    }
    while (true) {
        if (_cur.x + inc < 0) {
            /*left up*/
            if (_cur.y == 0) {
                /*to front*/
                _cur.y = 0;
                _cur.x = 0;
                return;
            } else {
                inc += _cur.x + 1;
                _cur.y -= 1;
                _cur.x = _lines[_cur.y].len;
            }
        } else if (_cur.x + inc > _lines[_cur.y].len) {
            /*right down*/
            if (_cur.y == _lines.size() - 1) {
                /*past last position*/
                _cur.y = _lines.size() - 1;
                _cur.x = _lines[_cur.y].len;
                return;
            } else {
                inc -= _lines[_cur.y].len - _cur.x + 1;
                _cur.y += 1;
                _cur.x = 0;
            }
        } else {
            /*in same line*/
            _cur.x += inc;
            return;
        }
    }
}

void TextBox::move_cursor_vertically(const int inc) {
    /*Move the cursor by inc up/down and consider line lengths and text beginning & end.*/
    if (!_app.is_shift_pressed()) {
        _invalidate_selection();
    }

    if (_cur.y + inc < 0) {
        /*to front*/
        _cur.y = 0;
        _cur.x = 0;
        return;
    } else if (_cur.y + inc >= _lines.size()) {
        /*past last position*/
        _cur.y = _lines.size() - 1;
        _cur.x = _lines[_cur.y].len;
        return;
    } else {
        /*normal movement*/
        _cur.y += inc;
        if (_cur.x > _lines[_cur.y].len) {
            _cur.x = _lines[_cur.y].len;
        }
    }
}

void TextBox::move_cursor_by_word(int n_words) {
    /*Move the cursor by n_words and consider line-lengths and text- starts & end.*/
    if (!_app.is_shift_pressed()) {
        _invalidate_selection();
    }
    while (n_words != 0) {
        const auto& line = _lines[_cur.y];
        const auto& buf = line.buf;
        if (n_words < 0) {
            /*go back*/
            int i = _cur.x - 1;
            while (i > 0 && !(buf[i - 1] == ' ' && buf[i] != ' ')) {
                --i;
            }
            move_cursor(i - _cur.x);
            ++n_words;
        } else {
            /*go forward*/
            int i = _cur.x + 1;
            while (i < line.len && !(buf[i - 1] != ' ' && buf[i] == ' ')) {
                ++i;
            }
            move_cursor(i - _cur.x);
            --n_words;
        }
    }
}

void TextBox::insert_char(const char c) {
    _invalidate_selection();

    auto& line = _lines[_cur.y];
    std::memmove(line.buf + _cur.x + 1, line.buf + _cur.x, line.len - _cur.x);
    line.buf[_cur.x] = c;

    ++_cur.x;
    ++line.len;
}

bool TextBox::_insert_text(const char* str) {
    {
        /*fail if the number of lines to be inserted exceeds the limits*/
        int n_lines = 0;
        const char* tmp = str;
        while ((tmp = std::strchr(tmp, '\n')) != nullptr) {
            ++n_lines;
            ++tmp;
        }
        if (_lines.size() + n_lines > this->max_n_lines) {
            return false;
        }
    }

    /*save initial line ending*/
    Line tmp;
    tmp.len = _lines[_cur.y].len - _cur.x;
    std::memcpy(tmp.buf, _lines[_cur.y].buf + _cur.x, tmp.len);
    _lines[_cur.y].len = _cur.x;

    /*copy lines*/
    int line_start = 0;
    int line_end = 0;
    while (str[line_end] != '\0') {
        if (str[line_end] == '\n') {
            std::memcpy(
                _lines[_cur.y].buf + _lines[_cur.y].len,
                str + line_start,
                line_end - line_start);
            _lines[_cur.y].len += line_end - line_start;
            ++_cur.y;
            _cur.x = 0;
            _lines.emplace(_lines.begin() + _cur.y);
            line_start = line_end + 1;
        }
        ++line_end;
    }

    /*last line*/
    auto& line = _lines[_cur.y];
    std::memcpy(line.buf + line.len, str + line_start, line_end - line_start);
    line.len += line_end - line_start;
    _cur.x = line_end + (line_start == 0 ? _cur.x : -line_start);
    std::memcpy(line.buf + line.len, tmp.buf, tmp.len);
    line.len += tmp.len;
    return true;
}

void TextBox::delete_chars(int n_chars) {
    /*Delete the given number characters from the text at the cursor's position.*/
    while (true) {
        auto& line = _lines[_cur.y];
        char* pos = line.buf + _cur.x;
        if (_cur.x + n_chars < 0) {
            /*left up*/
            if (_cur.y == 0) {
                n_chars = -_cur.x;
            } else {
                auto& line_above = _lines[_cur.y - 1];
                const auto new_col = line_above.len;
                std::memcpy(line_above.buf + line_above.len, pos, line.len - _cur.x);
                line_above.len += line.len - _cur.x;
                _lines.erase(_lines.begin() + _cur.y);
                n_chars += _cur.x + 1;
                --_cur.y;
                _cur.x = new_col;
            }
        } else if (_cur.x + n_chars > line.len) {
            /*right down*/
            if (_cur.y == _lines.size() - 1) {
                n_chars = line.len - _cur.x;
            } else {
                n_chars -= line.len - _cur.x + 1;
                auto& line_below = _lines[_cur.y + 1];
                std::memcpy(pos, line_below.buf, line_below.len);
                line.len = _cur.x + line_below.len;
                _lines.erase(_lines.begin() + _cur.y + 1);
            }
        } else {
            /*in same line*/
            if (n_chars < 0) {
                std::memmove(pos + n_chars, pos, line.len - _cur.x);
                _cur.x += n_chars;
                line.len += n_chars;
            } else {
                std::memmove(pos, pos + n_chars, line.len - _cur.x + n_chars);
                line.len -= n_chars;
            }
            return;
        }
    }
}

void TextBox::_delete_text(const TextCoord& start, const TextCoord& end) {
    const auto remaining_len = _lines[end.y].len - end.x;
    std::memmove(
        _lines[start.y].buf + start.x,
        _lines[end.y].buf + end.x,
        remaining_len);
    _lines[start.y].len = start.x + remaining_len;
    for (int i = start.y + 1; i <= end.y; ++i) {
        _lines.erase(_lines.begin() + end.y);
    }
}

bool TextBox::delete_selected_text() {
    if (_sel_start.y == -1 ||  _sel_start == _cur) {
        _invalidate_selection();
        return false;
    }
    const auto sel = _get_selection_bounds();
    _delete_text(sel.first, sel.second);
    _cur.y = sel.first.y;
    _cur.x = sel.first.x;
    _invalidate_selection();
    return true;
}

bool TextBox::_insert_newline() {
    if (_lines.size() >= this->max_n_lines) {
        return false;
    }
    auto& new_line = *_lines.emplace(_lines.begin() + _cur.y + 1);
    auto& line = _lines[_cur.y];
    char* pos = line.buf + _cur.x;

    new_line.len = line.len - _cur.x;
    line.len -= new_line.len;
    std::memcpy(new_line.buf, pos, new_line.len);
    ++_cur.y;
    _cur.x = 0;
    return true;
}

bool TextBox::has_focus() const {
    return _has_focus;
}

void TextBox::set_focus(const bool has_focus) {
    _has_focus = has_focus;
    this->draw();
}

void TextBox::draw() {
    _draw_background();

    XRectangle rect{
        static_cast<short>(this->_x() + _padding),
        static_cast<short>(this->_y() + _padding),
        static_cast<unsigned short>(this->_width() - 2 * _padding),
        static_cast<unsigned short>(this->_height() - 2 * _padding)};
    XSetClipRectangles(_app.dpy, _app.gc, 0, 0, &rect, 1, Unsorted);
    XftDrawSetClipRectangles(_app.xft_drawable, 0, 0, &rect, 1);

    IntCoord cur_pos(_calc_cursor_pos());
    _adjust_offset(cur_pos);
    _draw_selection();
    _draw_text();
    if (_has_focus) {
        _draw_cursor(cur_pos);
    }

    XSetClipMask(_app.dpy, _app.gc, None);
    XftDrawSetClip( _app.xft_drawable, 0);
}

int TextBox::handle_key_press(XEvent& evt) {
    if (!_has_focus) {
        return 0;
    }
    const int buf_size = 8;
    char buf[buf_size];

    switch (evt.xkey.keycode) {
        case 50: /*shift left*/
        case 62: /*shift right*/
            _start_selection();
            return 0;
        case 53: /*ctrl + x*/
            if (_app.is_ctrl_pressed()) {
                _write_selected_text_to_clipboard();
                delete_selected_text();
            } else {
                /*normal text input*/
                if (XLookupString(&evt.xkey, buf, buf_size, nullptr, nullptr) > 0) {
                    delete_selected_text();
                    insert_char(buf[0]);
                } else {
                    return 0;
                }
            }
            break;
        case 54: /*ctrl + c*/
            if (_app.is_ctrl_pressed()) {
                _write_selected_text_to_clipboard();
                return 0;
            } else {
                /*normal text input*/
                if (XLookupString(&evt.xkey, buf, buf_size, nullptr, nullptr) > 0) {
                    delete_selected_text();
                    insert_char(buf[0]);
                } else {
                    return 0;
                }
            }
            break;
        case 55: /*ctrl + v*/
            if (_app.is_ctrl_pressed()) {
                delete_selected_text();
                const auto text(::barn::x11::cp::get_text_from_clipboard());
                _insert_text(text.c_str());
            } else {
                /*normal text input*/
                if (XLookupString(&evt.xkey, buf, buf_size, nullptr, nullptr) > 0) {
                    delete_selected_text();
                    insert_char(buf[0]);
                } else {
                    return 0;
                }
            }
            break;
        case 22: /*backspace*/
            if (!delete_selected_text()) {
                delete_chars(-1);
            }
            break;
        case 119: /*delete*/
            if (!delete_selected_text()) {
                delete_chars(+1);
            }
            break;
        case 36: /*enter*/
            delete_selected_text();
            _insert_newline();
            break;
        case 111: /*up arrow key*/
            move_cursor_vertically(-1);
            break;
        case 116: /*down arrow key*/
            move_cursor_vertically(+1);
            break;
        case 113: /*left arrow key*/
            _app.is_ctrl_pressed() ? move_cursor_by_word(-1) : move_cursor(-1);
            break;
        case 114: /*right arrow key*/
            _app.is_ctrl_pressed() ? move_cursor_by_word(+1) : move_cursor(+1);
            break;
        case 110: /*home key*/
            _cur.x = 0;
            break;
        case 115: /*end key*/
            _cur.x = _lines[_cur.y].len;
            break;
        default:
            if (XLookupString(&evt.xkey, buf, buf_size, nullptr, nullptr) > 0) {
                /*normal text input*/
                delete_selected_text();
                insert_char(buf[0]);
            } else {
                return 0;
            }
    }
    return 0;
}

int TextBox::handle_key_release(XEvent& evt) {
    return 0;
}

Dialog::Dialog(App& app) : app(app)
{}

AppState Dialog::handle_key_press(XEvent& evt) {
    switch(evt.xkey.keycode) {
        case 9: /*esc*/
            return x11::AppState::EXIT;
        case 23: /*tab*/
            if (app.is_shift_pressed()) {
                return x11::AppState::BACK;
            }
            return x11::AppState::PROCEED;
        case 36: /*ctrl + enter*/
            if (this->app.is_ctrl_pressed()) {
                return x11::AppState::EXIT;
            }
            break;
    }
    return x11::AppState::KEEP_RUNNING;
}

AppState Dialog::handle_key_release(XEvent& evt) {
    return AppState::KEEP_RUNNING;
}

App::App(const std::shared_ptr<Settings>& settings, std::shared_ptr<Context>& context)
:
settings(settings),
context(context),
dpy(XOpenDisplay(nullptr)),
screen(DefaultScreen(dpy)),
root_win(RootWindow(dpy, screen)),
win(setup_window()),
gc(XCreateGC(dpy, win, 0, 0)),
xft_drawable(XftDrawCreate(dpy, win, DefaultVisual(dpy, 0), DefaultColormap(dpy, 0))),
font(XftFontOpen(
    dpy,
    screen,
    XFT_FAMILY,
    XftTypeString,
    "monospace",
    XFT_SIZE,
    XftTypeDouble,
    font_size,
    nullptr)),
line_height(font->ascent + font->descent),
bc_app(0x222222),
fc_app_frame(0xbbbbbb),
fc_text(alloc_color({65535, 65535, 65535, 0})),
fc_label(alloc_color({30000, 30000, 30000, 65535})),
fc_hint(alloc_color({30000, 30000, 30000, 65535})) {
    XSetStandardProperties(
        this->dpy,
        this->win,
        "Barn's Bookmark Manager",
        "",
        None,
        nullptr,
        0,
        nullptr);
}

App::~App() {
    free_color(this->fc_label);
    free_color(this->fc_text);
    free_color(this->fc_hint);

    XFreeGC(this->dpy, this->gc);
    XDestroyWindow(this->dpy, this->win);
    XCloseDisplay(this->dpy);
}

Window App::setup_window() {
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    attrs.background_pixel = this->bc_app;
    attrs.event_mask =
        ExposureMask
        | KeyPressMask
        | KeyReleaseMask
        | VisibilityChangeMask;

    return XCreateWindow(
        this->dpy,
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
            this->dpy,
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

int App::resize_window(const int rows, const int cols) {
    const Screen* const screen DefaultScreenOfDisplay(this->dpy);
    this->win_width = cols * this->font->max_advance_width;
    this->win_height = rows * this->line_height;
    return XMoveResizeWindow(
        this->dpy,
        this->win,
        (screen->width - this->win_width) / 2,
        (screen->height - this->win_height) / 3,
        this->win_width,
        this->win_height);
}

XGlyphInfo App::get_text_extents(const std::string& str) {
    XGlyphInfo extents;
    XftTextExtents8(
        this->dpy,
        this->font,
        reinterpret_cast<const XftChar8*>(str.c_str()),
        str.length(),
        &extents);
    return extents;
}

void App::draw_rect_frame() {
    XSetForeground(this->dpy, this->gc, this->fc_app_frame);
    XDrawRectangle(this->dpy, this->win, this->gc, 0, 0, this->win_width - 1, this->win_height -1 );
}

void App::draw_text(
        const XftColor& color,
        const std::string& str,
        const float row,
        const float col,
        const int y_px_offset,
        const int x_px_offset) {
    XftDrawStringUtf8(
        this->xft_drawable,
        &color,
        this->font,
        col * this->font->max_advance_width + x_px_offset,
        (row + 1) * this->line_height + y_px_offset,
        (FcChar8*)str.c_str(),
        str.length());
}

void App::redraw() {
    XClearWindow(this->dpy, this->win);
    draw_rect_frame();
    (*dialog_it)->draw();
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
    return (*dialog_it)->handle_key_press(evt);
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
    return (*dialog_it)->handle_key_release(evt);
}

void App::run() {
    dialog_it = dialogs.begin();
    if (!grab_keyboard()) {
        return;
    } else if (dialog_it == dialogs.end()) {
        log(WARN) << "Past last dialog." << std::endl;
        return;
    }
    XMapRaised(this->dpy, this->win);

    XEvent evt;
    AppState state = AppState::KEEP_RUNNING;
    while (state != AppState::EXIT && !XNextEvent(this->dpy, &evt)) {
        switch (evt.type) {
            case Expose:
                if (evt.xexpose.count == 0) {
                    redraw();
                }
                break;
            case VisibilityNotify:
                if (evt.xvisibility.state != VisibilityUnobscured) {
                    XRaiseWindow(this->dpy, this->win);
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
                (*dialog_it)->is_initalized = false;
                ++dialog_it;
                if (dialog_it == dialogs.end()) {
                    state = AppState::EXIT;
                    break;
                }
                state = AppState::KEEP_RUNNING;
                redraw();
                break;
            case AppState::BACK:
                if (dialog_it != dialogs.begin()) {
                    (*dialog_it)->is_initalized = false;
                    --dialog_it;
                    state = AppState::KEEP_RUNNING;
                    redraw();
                }
                break;
        }
    }
}

XftColor App::alloc_color(const XRenderColor& color) {
    XftColor xft_color;
    XftColorAllocValue(
        this->dpy,
        DefaultVisual(this->dpy, this->screen),
        DefaultColormap(this->dpy, this->screen),
        &color,
        &xft_color);
    return xft_color;
}

void App::free_color(XftColor& xft_color) {
    XftColorFree(
        this->dpy,
        DefaultVisual(this->dpy, this->screen),
        DefaultColormap(this->dpy, this->screen),
        &xft_color);
}

} // namespace x11
} // namespace bbm
} // namespace barn
