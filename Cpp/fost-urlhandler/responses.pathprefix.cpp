/**
    Copyright 2011-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>

#include <fost/log>


namespace {


    const fostlib::module c_mod(fostlib::c_fost_web_urlhandler, "pathprefix");


    const class pathprefix : public fostlib::urlhandler::view {
      public:
        pathprefix() : view("fost.view.pathprefix") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            if (not configuration.has_key("")) {
                fostlib::log::warning(c_mod)(
                        "",
                        "Missing default key. There should always be a view "
                        "for \"\"")("configuration", configuration);
            }
            std::pair<std::size_t, fostlib::string> longest(
                    0, fostlib::string());
            for (fostlib::json::const_iterator p(configuration.begin());
                 p != configuration.end(); ++p) {
                fostlib::string key = fostlib::coerce<fostlib::string>(p.key());
                std::size_t length = key.code_points();
                if (path.substr(0, length) == key && length > longest.first) {
                    longest = std::make_pair(length, key);
                }
            }

            if (configuration.has_key(longest.second)) {
                return execute(
                        configuration[longest.second],
                        path.substr(longest.first), req, h);
            } else {
                return execute(
                        fostlib::json{"fost.response.404"}, path, req, h);
            }
        }
    } c_pathprefix;


}
