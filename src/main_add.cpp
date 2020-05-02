/* Entry point of Barn's Bookmark Manager Add bookmark program.

author: andreasl
*/
#include "add.hpp"
#include "add_settings.hpp"
#include "bookmark.hpp"
#include "cli.hpp"
#include "datetime.hpp"
#include "log.hpp"

#include <utility>
#include <memory>
#include <vector>

int main(int argc, const char* argv[]) {
    using namespace ::barn::bbm;

    const auto options = parse_options(argc,argv);
    const auto settings = std::make_shared<Settings>(load_settings(options.settings_path));
    std::string url;
    std::string title;
    if (!fetch_url_and_title(url, title)) {
        log(ERROR) << "Could not fetch url and title." << std::endl;
        return exitcode::WRONG_INPUT;
    }
    auto context = std::make_shared<Context>(Context{{std::move(url), std::move(title)}});
    x11::App app(settings, context);
    app.dialogs.emplace_back(std::make_shared<AddPathDialog>(app));
    app.run();

    if (context->do_store) {
        save_bookmark(std::move(context->bookmark), settings->bookmarks_root_path / "subpath");
    }
    return exitcode::SUCCESS;
}
