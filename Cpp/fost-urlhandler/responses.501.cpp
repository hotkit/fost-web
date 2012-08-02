/*
    Copyright 2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_501 : public fostlib::urlhandler::view {
    public:
        response_501()
        : view("fost.response.501") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &, const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        L"<html><head><title>Not Implemented</title></head>"
                            L"<body><h1>Not Implemented</h1></body></html>",
                        fostlib::mime::mime_headers(), L"text/html" ));
            return std::make_pair(response, 501);
        }
} c_response_501;

const fostlib::urlhandler::view &fostlib::urlhandler::response_501 =c_response_501;
