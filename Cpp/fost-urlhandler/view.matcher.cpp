/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/log>
#include "fost/matcher.hpp"


namespace {

    const class view_matcher : public fostlib::urlhandler::view {
      public:
        view_matcher() : view("fost.view.match") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            /*
            Execute the view that match with the defined path,
            If matched, then set the headers __1, __2, ... so the underlying
            views can use it. if not matched, the fallback view should be
            executed.

            Example: {
                "view": "fost.view.match",
                "configuration": {
                    "match": [{
                        "path": ["/test", 1],
                        "execute": "<view>"
                    }],
                    "no-match": "<view>" // fallback
                }
            }

            * This view doesn't consume any path
            */
            if (!configuration.has_key("no-match")
                || !configuration.has_key("match")) {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "fost.view.match required keys [\"no-match\", "
                        "\"match\"] in the"
                        "configuration \"\"");
            }
            auto &match_config = configuration["match"];
            fostlib::log::debug(fostlib::c_fost_web_urlhandler)("path", path)(
                    "configuration", match_config);

            auto matched = fostlib::matcher(match_config, path);
            if (matched) {
                for (std::size_t i = 0; i != matched.value().arguments.size();
                     i++) {
                    req.headers().set(
                            f5::u8string{"__"}
                                    + fostlib::coerce<fostlib::string>(i + 1),
                            matched.value().arguments[i]);
                }
                fostlib::log::debug(fostlib::c_fost_web_urlhandler)(
                        "matched",
                        path)("configuration", matched.value().configuration);
                return execute(
                        matched.value().configuration["execute"], path, req,
                        host);
            }
            return execute(configuration["no-match"], path, req, host);
        }
    } c_view_matcher;


}


const fostlib::urlhandler::view &fostlib::urlhandler::view_matcher =
        c_view_matcher;
