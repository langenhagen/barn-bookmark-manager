/* Implementation for add.hpp.

author: andreasl
*/
#include "datetime.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace barn {
namespace bbm {

/*Constructor*/
DateTime::DateTime(int year_, int month_, int day_of_month_, int hour_, int minute_)
    : year(year_), month(month_), day_of_month(day_of_month_), hour(hour_), minute(minute_)
{}

/*Constructor from string.*/
DateTime::DateTime(const std::string& str, const char* fmt) {
    std::stringstream ss(str);
    std::tm tm;
    ss >> std::get_time(&tm, fmt);

    this->year = tm.tm_year + 1900;
    this->month = tm.tm_mon + 1;
    this->day_of_month = tm.tm_mday;
    this->hour = tm.tm_hour;
    this->minute = tm.tm_min;
}

/*Convert to std::tm struct.*/
std::tm DateTime::tm() const {
    std::tm tm;
    tm.tm_year = this->year - 1900;
    tm.tm_mon = this->month - 1;
    tm.tm_mday = this->day_of_month;
    tm.tm_hour = this->hour;
    tm.tm_min = this->minute;
    return tm;
}

/*Convert to c string.*/
void DateTime::c_str(char* str, const size_t max_size, const char* fmt) const {
    std::tm tm(this->tm());
    std::strftime(str, max_size, fmt, &tm);
}

/*Convert to string.*/
std::string DateTime::str(const char* fmt) const {
    char dt_c_str[255];
    this->c_str(dt_c_str, sizeof(dt_c_str), fmt);
    return std::string(dt_c_str);
}

} // namespace bbm
} // namespace barn