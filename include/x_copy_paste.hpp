/* Utility functions for easy reading from and writing to the x window system clipboard.

Use the external utility `xclip` rather than implemting clipboard capabilities for the X
application. This serves 2 purposes. First, avoid the necessity for the caller to implement an x11
application and avoid the complex setup for clipboard capabilities in x window applications. Second,
allow access to the clipboard after the calling process terminated.

author: andreasl
*/
#pragma once

#include <string>

namespace barn {
namespace x11 {
namespace cp {

/*Write the given string to the x window clipboard.*/
void write_to_clipboard(const std::string& str);

/*Return a string from the x window clipboard.*/
std::string get_text_from_clipboard();

} // namespace cp
} // namespace x11
} // namespace barn
