/* User-friendly datetime representation.

author: andreasl
*/
#pragma once

#include <ctime>
#include <string>

namespace barn {
namespace bbm {

/*Simple datetime structure that can parse and output simple datetime strings.*/
struct DateTime {
    int year;           /*4 digit year, e.g. 2020.*/
    int month;          /*Number of mont in [1, 12].*/
    int day_of_month;   /*Number of day in [1, 31].*/
    int hour;           /*Hour of day in 24-hours in [0, 23].*/
    int minute;         /*Minute of hour in [0, 59].*/

    /*Constructor*/
    explicit DateTime(int year_, int month_, int day_of_month_, int hour_ = 0, int minute_ = 0);

    /*Constructor from string.*/
    explicit DateTime(const std::string& str, const char* fmt = "%Y-%m-%d %H:%M");

    /*Convert to std::tm struct.*/
    std::tm tm() const;

    /*Convert to c string.*/
    void c_str(char* str, const size_t max_size, const char* fmt = "%Y-%m-%d %H:%M") const;

    /*Convert to string.*/
    std::string str(const char* fmt = "%Y-%m-%d %H:%M") const;
};

} // namespace bbm
} // namespace barn
