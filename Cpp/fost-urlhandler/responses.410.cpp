/*
    Copyright 2011-2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_410 : public fostlib::urlhandler::view {
  public:
    response_410() : view("fost.response.410") {}

    std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &,
            const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &) const {
        boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                L"<html><head><title>Gone</title></head>"
                L"<body><h1>Gone</h1></body></html>",
                fostlib::mime::mime_headers(), L"text/html"));
        return std::make_pair(response, 410);
    }
} c_response_410;

const fostlib::urlhandler::view &fostlib::urlhandler::response_410 =
        c_response_410;
