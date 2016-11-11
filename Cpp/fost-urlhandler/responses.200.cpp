/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_200 : public fostlib::urlhandler::view {
    public:
        response_200()
        : view("fost.response.200") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &, const fostlib::string &,
            fostlib::http::server::request &,
            const fostlib::host &
        ) const {
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        L"<html><head><title>OK</title></head>"
                            L"<body><h1>OK</h1></body></html>",
                        fostlib::mime::mime_headers(), L"text/html" ));
            return std::make_pair(response, 200);
        }
} c_response_200;

const fostlib::urlhandler::view &fostlib::urlhandler::response_200 =c_response_200;
