/* Business logic for BBM Fetch app.

author: andreasl
*/
#pragma once

#include "fetch_settings.hpp"
#include "x_app.hpp"

namespace barn {
namespace bbm {

/*Fetch bookmark x11 window application.*/
struct FetchBookmarkApp : x11::App {
    FetchSettings settings;  /*Application settings.*/

    FetchBookmarkApp(const FetchSettings& settings); /*Constructor.*/
};

} // namespace bbm
} // namespace barn
