#include <iostream>
#include <sstream>
#include <string>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <version_info.hpp>

Display *dis;
int screen;
Window win;
GC gc;

void init_x() {
    /* get the colors black and white (see section for details) */
    unsigned long black,white;

    /* use the information from the environment variable DISPLAY
       to create the X connection:
    */
    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    black=BlackPixel(dis,screen),   /* get color black */
    white=WhitePixel(dis, screen);  /* get color white */

    /* once the display is initialized, create the window.
       This window will be have be 200 pixels across and 300 down.
       It will have the foreground white and background black
    */
    win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,
        200, 300, 0, white, black);

    /* here is where some properties of the window can be set.
       The third and fourth items indicate the name which appears
       at the top of the window and the name of the minimized window
       respectively.
    */
    XSetStandardProperties(dis,win,"My Window","HI!",None,NULL,0,NULL);

    /* this routine determines which types of input are allowed in
       the input.  see the appropriate section for details...
    */
    XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);

    /* create the Graphics Context */
    gc=XCreateGC(dis, win, 0,0);

    /* here is another routine to set the foreground and background
       colors _currently_ in use in the window.
    */
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    /* clear the window and bring it on top of the other windows */
    XClearWindow(dis, win);
    XMapRaised(dis, win);
};

void close_x() {
    /* it is good programming practice to return system resources to the
       system...
    */
    XFreeGC(dis, gc);
    XDestroyWindow(dis,win);
    XCloseDisplay(dis);
    exit(1);
}

unsigned long chartreuse;

void redraw(const std::string& str) {
    XClearWindow(dis, win);
    XDrawString(dis,win,gc,10,20,str.c_str(),str.size());
}

void get_colors() {
    XColor tmp;

    XParseColor(dis, DefaultColormap(dis,screen), "chartreuse", &tmp);
    XAllocColor(dis,DefaultColormap(dis,screen),&tmp);
    chartreuse=tmp.pixel;
};

std::stringstream ss;
std::string str = "Hi There!";

int main(int argc, const char* argv[]) {
    std::string build_timestamp = bbm::get_build_timestamp();
    std::cout << "Hello, World!\nVersion: " << build_timestamp << std::endl;

    init_x();

    XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);

    XEvent event;       /* the XEvent declaration !!! */
    KeySym key;     /* a dealie-bob to handle KeyPress Events */
    char text[255];     /* a char buffer for KeyPress Events */

    // get_colors();
    // XSetForeground(dis,gc,chartreuse);
    unsigned long white = WhitePixel(dis, screen);
    XSetForeground(dis,gc,white);

    /* look for events forever... */
    while(1) {
        /* get the next event and stuff it into our event variable.
           Note:  only events we set the mask for are detected!
        */
        XNextEvent(dis, &event);

        if (event.type==Expose && event.xexpose.count==0) {
            /* the window was exposed redraw it! */
            redraw(str);
        }
        if (event.type==KeyPress&&
            XLookupString(&event.xkey,text,255,&key,0)==1) {
            /* use the XLookupString routine to convert the invent
               KeyPress data into regular text.  Weird but necessary...
            */
            if (text[0]=='q') {
                close_x();
            }
            // std::stringstream ss;
            ss << text[0];
            // ss << "You pressed the " << text[0] << " key!\n";
            redraw(ss.str());
        }
        if (event.type==ButtonPress) {
            /* tell where the mouse Button was Pressed */
            // std::stringstream ss;
            ss << "You pressed a button at (" << event.xbutton.x << "," << event.xbutton.y <<")\n";
            redraw(ss.str());
        }
    }

    close_x();

    return 0;
}
