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
                                            "precondition": [],
                                            "execute": "<view>"
                                    },{ // fallback
                                            "path": [],
                                            "execute": "<view>"
                                    }]
                    }
            }
            */
            auto &match_config = configuration["match"];
            fostlib::log::debug(fostlib::c_fost_web_urlhandler)("path", path);

            for (auto m : match_config) {
                auto matched = fostlib::matcher(m, path);
                if (matched) {
                    for (std::size_t i = 0;
                         i != matched.value().arguments.size(); i++) {
                        req.headers().set(
                                f5::u8string{"__"}
                                        + fostlib::coerce<fostlib::string>(
                                                i + 1),
                                matched.value().arguments[i]);
                    }
                    return execute(m["execute"], path, req, host);
                }
            }
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "fost.view.match not implemented");
        }
    } c_view_matcher;


}


const fostlib::urlhandler::view &fostlib::urlhandler::view_matcher =
        c_view_matcher;
