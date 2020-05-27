/**
    Copyright 2018-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const fostlib::module c_mod(fostlib::c_fost_web_urlhandler, "pathname");


    const class pathname : public fostlib::urlhandler::view {
      public:
        pathname() : view("fost.view.pathname") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            if (configuration.has_key(path)) {
                /// The path is fully consumed by this view because it
                /// matches in its entirety
                return execute(configuration[path], "", req, h);
            } else {
                return execute(
                        fostlib::json("fost.response.404"), path, req, h);
            }
        }
    } c_pathname;


}
