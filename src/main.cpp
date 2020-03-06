/* based on: http://mech.math.msu.su/~nap/2/GWindow/xintro.html
   window decorations displayabled based on:
   https://stackoverflow.com/questions/31361859/simple-window-without-titlebar
     - also helpful for possible program structure

- use the command line tool `xev` to find out x11 key codes

- https://tronche.com/gui/x/xlib/events/structures.html
- https://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XKeyEvent

- https://tronche.com/gui/x/xlib/window-information/properties-and-atoms.html
*/
#include <iostream>
#include <sstream>
#include <string>

/* order of X11 includes allegedlly important*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <version_info.hpp>

Display *display;
int screen;
Window window;
GC gc;

int i = 0;
std::stringstream ss;
std::string str = "Hi There!";

void init_x() {

    /* use the information from the environment variable DISPLAY
       to create the X connection:*/
    display = XOpenDisplay(nullptr);
    screen = DefaultScreen(display);

    /* get the colors black and white*/
    unsigned long black = BlackPixel(display, screen);
    unsigned long white = WhitePixel(display, screen);

    /* once the display is initialized, create the window.*/
    window = XCreateSimpleWindow(
        display,
        DefaultRootWindow(display),
        100 /*pos x*/,
        100 /*pos y*/,
        400 /*width*/,
        200 /*height*/,
        0 /*border width*/,
        white /*border*/,
        black /*background*/);

    /* here is where some properties of the window can be set.
       The third and fourth items indicate the name which appears
       at the top of the window and the name of the minimized window
       respectively.
    */
    XSetStandardProperties(
        display,
        window,
        "My Window",
        "HI!",
        None,
        nullptr,
        0,
        nullptr);

    /* disable window decorations
       based on https://stackoverflow.com/questions/31361859/simple-window-without-titlebar
       I had however to replace  _NET_WM_WINDOW_TYPE  with  _MOTIF_WM_HINTS
    */
    Atom window_type = XInternAtom(
        display,
        "_MOTIF_WM_HINTS" /*atom name*/,
        False /*only if exists*/);
    Atom value = XInternAtom(
        display,
        "_NET_WM_WINDOW_TYPE_DOCK" /*atom name*/,
        False /*only if exists*/);
    XChangeProperty(
        display,
        window,
        window_type,
        XA_ATOM,
        32,
        PropModeAppend,
        (unsigned char*)&value,
        1);

    /* this routine determines which types of input are allowed in
       the input.  see the appropriate section for details...
    */
    XSelectInput(display, window, ExposureMask|ButtonPressMask|KeyPressMask);

    /* create the Graphics Context */
    gc = XCreateGC(display, window, 0, 0);

    /* here is another routine to set the foreground and background
       colors _currently_ in use in the window.
    */
    XSetBackground(display, gc, white);
    XSetForeground(display, gc, black);

    /* clear the window and bring it on top of the other windows */
    XClearWindow(display, window);
    XMapRaised(display, window);
};

void close_x() {
    /* it is good programming practice to return system resources to the system...*/
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}


void redraw(const std::string& str) {
    std::cout << "redrawing " << ++i << std::endl;
    XClearWindow(display, window);
    XDrawString(
        display,
        window,
        gc,
        10 /*pos x*/,
        20 /*pos y*/,
        str.c_str(),
        str.size());
}


int main(int argc, const char* argv[]) {
    std::string build_timestamp = bbm::get_build_timestamp();
    std::cout << "Hello, World!\nVersion: " << build_timestamp << std::endl;

    init_x();

    unsigned long white = WhitePixel(display, screen);
    XSetForeground(display, gc, white);  // dunno but necessary

    /* listen to given event types*/
    XSelectInput(
        display,
        window,
        ExposureMask /*when portions of the window are exposed that were hidden by another window*/
        | ButtonPressMask
        | KeyPressMask);


    XEvent event;  /* the XEvent declaration */
    const int buffer_length = 32;
    char text_buffer[buffer_length];  /* a char buffer for KeyPress Events, the size is arbitrary */


    while(true) {
        /* get the next event and stuff it into our event variable.
           Note: only events we set the mask for are detected!
        */
        XNextEvent(display, &event);

        if (event.type == Expose && event.xexpose.count == 0) {
            /* the window was exposed redraw it!
               see: https://tronche.com/gui/x/xlib/events/exposure/expose.html*/
            redraw(str);
        }
        else if (event.type == KeyPress &&
            XLookupString(
                &event.xkey /*event struct*/,
                text_buffer /*output buffer*/,
                buffer_length /*buffer length*/,
                nullptr /*output keysym or nullptr*/,
                nullptr /*status_in_out or nullptr*/) == 1) {
            /* use the XLookupString routine to convert the KeyPress data into regular text.
               see: https://tronche.com/gui/x/xlib/utilities/XLookupString.html
            */
            std::cout << event.xkey.keycode;
            if( event.xkey.keycode == 9 /*esc*/) {
                break;
            }
            else if( event.xkey.keycode == 22 /*delete*/) {
                const std::string str = ss.str();
                const std::string substr = str.substr(0, str.size()-1);
                ss.str(std::string());
                ss << substr;
            }
            else {
                /* normal text input*/
                ss << text_buffer[0];
            }
            redraw(ss.str());
        }
        else if (event.type == ButtonPress) {
            /* tell where the mouse Button was pressed */
            // std::stringstream ss;
            ss.str(std::string());
            ss << "You pressed a mouse button at ("
                << event.xbutton.x
                << ","
                << event.xbutton.y
                << ")\n";
            redraw(ss.str());
        }
    }

    close_x();
    return 0;
}
