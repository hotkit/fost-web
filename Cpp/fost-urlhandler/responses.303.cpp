/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_303 : public fostlib::urlhandler::view {
  public:
    response_303() : view("fost.response.303") {}

    std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &config,
            const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &) const {
        if (!config.has_key("location"))
            throw fostlib::exceptions::not_implemented(
                    "Configuration 'location' must give a full URL to redirect "
                    "to");
        fostlib::string location =
                fostlib::coerce<fostlib::string>(config["location"]);
        if (location.empty())
            throw fostlib::exceptions::not_implemented(
                    "Configuration 'location' must give a full URL to redirect "
                    "to");
        fostlib::mime::mime_headers headers;
        headers.set("Location", location);
        boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                L"<html><head><title>See Other</title></head>"
                L"<body><h1>See Other</h1></body></html>",
                headers, L"text/html"));
        return std::make_pair(response, 303);
    }
} c_response_303;

const fostlib::urlhandler::view &fostlib::urlhandler::response_303 =
        c_response_303;
