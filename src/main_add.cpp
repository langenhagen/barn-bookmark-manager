/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "add.hpp"
#include "add_settings.hpp"
#include "bookmark.hpp"
#include "cli.hpp"
#include "datetime.hpp"
#include "log.hpp"

#include <experimental/filesystem>
#include <utility>
#include <memory>
#include <vector>

int main(int argc, const char* argv[]) {
    namespace fs = std::experimental::filesystem;
    using namespace ::barn::bbm;

    const auto options = parse_options(argc,argv);
    const auto settings = std::make_shared<Settings>(load_settings(options.settings_path));
    std::string url;
    std::string title;
    if (!fetch_url_and_title(url, title)) {
        log(ERROR) << "Could not fetch url and title." << std::endl;
        return exitcode::WRONG_INPUT;
    }
    auto context = std::make_shared<Context>(Context{{url, std::move(title)}});
    {
        x11::App app(settings, context);
        add_dialogs_to_app(app);
        app.run();
    }

    if (!context->do_store) {
        return exitcode::SUCCESS;
    }
    if (context->keep_querystring == false) {
        context->bookmark.url = remove_querystring(context->bookmark.url);
    }
    fs::path path = save(
        std::move(context->bookmark),
        settings->bookmarks_root_path / context->bookmark_dir);
    if (path.empty()) {
        return exitcode::SYSTEM_ERROR;
    }
    if (settings->download_websites) {
        save_website_text(url, path.replace_extension(".website.txt"));
    }
    return exitcode::SUCCESS;
}
