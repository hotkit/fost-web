/**
    Copyright 2012-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class response_302 : public fostlib::urlhandler::view {
      public:
        response_302() : view("fost.response.302") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &,
                fostlib::http::server::request &req,
                const fostlib::host &) const {
            if (!config.has_key("location"))
                throw fostlib::exceptions::not_implemented(
                        "Configuration 'location' must give a URL to redirect "
                        "to");
            fostlib::string location =
                    fostlib::coerce<fostlib::string>(config["location"]);
            if (location.empty())
                throw fostlib::exceptions::not_implemented(
                        "Configuration 'location' must give a URL to redirect "
                        "to");
            fostlib::mime::mime_headers headers;
            headers.set("Location", location);
            boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    "<html><head><title>Found</title></head>"
                    "<body><h1>Found</h1></body></html>",
                    headers, "text/html"));
            return std::make_pair(response, 302);
        }
    } c_response_302;


}


const fostlib::urlhandler::view &fostlib::urlhandler::response_302 =
        c_response_302;
