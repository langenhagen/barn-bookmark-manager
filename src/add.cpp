/* Implementation for add.hpp.

author: andreasl
*/
#include "add.hpp"

#include "add_settings.hpp"
#include "bookmark.hpp"
#include "log.hpp"
#include "x_copy_paste.hpp"

#include "yaml-cpp/yaml.h"

#include <chrono>
#include <cstring>
#include <experimental/filesystem>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>

extern "C" {
#include <xdo.h>
}

namespace fs = std::experimental::filesystem;

namespace barn {
namespace bbm {

namespace {

/*Create random id to a given string.*/
static const std::string generate_id() {
    std::stringstream ss;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 35);
    constexpr static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 6; ++i) {
        ss << alphanum[dis(gen)];
    }
    return ss.str();
}

} // namespace

/*Fetch the url and the website tile from the focused Chrome top window.*/
bool fetch_url_and_title(std::string& url, std::string& title) {
    std::unique_ptr<xdo_t, decltype(&xdo_free)> xdo(xdo_new(nullptr), &xdo_free);

    Window win;
    xdo_get_active_window(xdo.get(), &win);

    unsigned char* win_name;
    int name_len;
    int win_type;
    xdo_get_window_name(xdo.get(), win, &win_name, &name_len, &win_type);

    constexpr const unsigned char chrome_win_suffix[17] = " - Google Chrome";
    if (std::memcmp(win_name + name_len - 16, chrome_win_suffix, 16)) {
        log(ERROR) << "Window \"" << win_name << "\" is not a Google Chrome window." << std::endl;
        return false;
    }

    /*focus the chrome address bar*/
    xdo_send_keysequence_window(xdo.get(), win, "Control_L+l", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); /*sleeping seems necessary*/

    /*copy the contents*/
    xdo_send_keysequence_window(xdo.get(), win, "Control_L+c", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    url = ::barn::x11::cp::get_text_from_clipboard();
    title = std::string(reinterpret_cast<const char*>(win_name));
    return true;
}

/*Store Bookmark on disk.*/
bool save_bookmark(const Bookmark& bookmark, const AddSettings& settings, const fs::path& subpath) {

    /*create bookmark folder*/
    fs::path bookmark_folder(settings.bookmarks_root_path / subpath);
    try {
        if (fs::exists(bookmark_folder)) {
            log(ERROR) << "Bookmark folder \"" << bookmark_folder << "\" does already exist."
                << std::endl;
            return false;
        }
        fs::create_directories(bookmark_folder);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not read or create directory \"" << bookmark_folder << "\"\n"
            << e.what() << std::endl;
        return false;
    }

    /*create yaml*/
    YAML::Emitter yaml;
    yaml.SetIndent(4);
    yaml << YAML::BeginMap
        << YAML::Key << "name" << YAML::Value << bookmark.name
        << YAML::Key << "url" << YAML::Value << bookmark.url
        << YAML::Key << "created" << YAML::Value << bookmark.created.str()
        << YAML::Key << "last_access" << YAML::Value << bookmark.last_access.str()
        << YAML::Key << "rating" << YAML::Value << bookmark.rating
        << YAML::Key << "comment" << YAML::Value << YAML::Literal << bookmark.comment
        << YAML::Key << "tags" << YAML::Value << YAML::Flow << YAML::BeginSeq;
        for( const std::string& tag : bookmark.tags) {
            yaml << tag;
        }
        yaml << YAML::EndSeq << YAML::EndMap;

    /*write yaml*/
    const std::string filename = generate_id() + ".yaml";
    const fs::path file = bookmark_folder / filename;
    std::ofstream out(file);
    out << yaml.c_str();
    if (!out) {
        log(ERROR) << "Could not write bookmark to file: " << file << std::endl;
        return false;
    }
    return true;
}

} // namespace bbm
} // namespace barn
