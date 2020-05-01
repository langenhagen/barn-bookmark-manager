/* Implementation for add.hpp.

author: andreasl
*/
#include "add.hpp"

#include "add_settings.hpp"
#include "bookmark.hpp"
#include "log.hpp"
#include "x_app.hpp"
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
const std::string generate_id() {
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

/*Write Yaml to file.*/
bool write(const YAML::Emitter& yaml, const fs::path& directory) {
    const std::string filename = generate_id() + ".yaml";
    const fs::path file = directory / filename;
    try {
        if (fs::exists(file)) {
            log(WARN) << "Bookmark file " << file << " already exists. Writing with new id..."
                << std::endl;
            return write(yaml, directory);
        }
    } catch (const std::exception& e) {
        log(ERROR) << "Could not read or create file " << file << "\n"
            << e.what() << std::endl;
        return false;
    }
    std::ofstream out(file);
    out << yaml.c_str();
    if (!out) {
        log(ERROR) << "Could not write bookmark to file: " << file << std::endl;
        return false;
    }
    return true;
}

} // namespace

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

bool save_bookmark(const Bookmark& bookmark, const fs::path& directory) {
    /*create bookmark folder*/
    try {
        fs::create_directories(directory);
    } catch (const std::exception& e) {
        log(ERROR) << "Could not read or create directory " << directory << "\n"
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

    return write(yaml, directory);
}

AddBookmarkApp::AddBookmarkApp(const AddSettings& s) : settings(s)
{}

AddCommentDialog::AddCommentDialog(x11::App& app) : Dialog(app)
{}

void AddCommentDialog::draw() {
    // TODO
}
int AddCommentDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}
int AddCommentDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

AddPathDialog::AddPathDialog(x11::App& app) : Dialog(app)
{}

void AddPathDialog::draw() {
    // TODO
}
int AddPathDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}

int AddPathDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

AddRatingDialog::AddRatingDialog(x11::App& app) : Dialog(app)
{}

void AddRatingDialog::draw() {
    // TODO
}

int AddRatingDialog::handle_key_press(XEvent& evt) {
    // TODO
    return 0;
}

int AddRatingDialog::handle_key_release(XEvent& evt) {
    // TODO
    return 0;
}

AddTagsDialog::AddTagsDialog(x11::App& app) : Dialog(app)
{}

void AddTagsDialog::draw() {
    // TODO
}

int AddTagsDialog::handle_key_press(XEvent& evt) {
    // TODO
}

int AddTagsDialog::handle_key_release(XEvent& evt) {
    // TODO
}

} // namespace bbm
} // namespace barn
