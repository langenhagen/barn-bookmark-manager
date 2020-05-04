/* Common bookmark representation.

author: andreasl
*/
#pragma once

#include "datetime.hpp"

#include <memory>
#include <string>
#include <vector>

namespace barn {
namespace bbm {

/*Stores textual content fetched from a web page.*/
struct WebpageContent {
    std::string title;  /*Title of the web page.*/
    std::string content;  /*Condensed textual content of a web page.*/
    DateTime fetch_time;  /*Timestamp when the data has been downloaded.*/
};

/*Representation of a bookmark.*/
struct Bookmark {
    std::string url;  /*Target URL.*/
    std::string title;  /*Optional title of the bookmark.*/
    DateTime created;  /*Creation timestamp.*/
    DateTime last_access;  /*Optional last access time; fields 0 if invalid.*/
    int rating;  /*Optional 1-5 star rating; 0 means not rated.*/
    std::vector<std::string> tags;  /*User-provided optional tags.*/
    std::string comment;  /*User-provided optional comment.*/
    std::shared_ptr<WebpageContent> content;  /*Optional webpage text content; null if missing.*/
};

} // namespace bbm
} // namespace barn
