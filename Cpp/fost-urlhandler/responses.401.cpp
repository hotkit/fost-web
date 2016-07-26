/*
    Copyright 2012-2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_401 : public fostlib::urlhandler::view {
    public:
        response_401()
        : view("fost.response.401") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &config, const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            if ( !config.has_key("schemes") )
                throw fostlib::exceptions::not_implemented(__func__,
                    "No 'schemes' key in the configuration so can't build "
                    "the required WWW-Authenticate header");
            fostlib::string schemes;
            for ( fostlib::json::const_iterator
                    m(config["schemes"].begin()); m != config["schemes"].end(); ++m ) {
                if ( schemes.empty() )
                    schemes = fostlib::coerce<fostlib::string>(m.key());
                else
                    schemes += ", " + fostlib::coerce<fostlib::string>(m.key());
            }
            fostlib::mime::mime_headers headers;
            headers.set("WWW-Authenticate", schemes);
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        L"<html><head><title>Not authorized</title></head>"
                            L"<body><h1>Not authorized</h1></body></html>",
                        headers, L"text/html" ));
            return std::make_pair(response, 401);
        }
} c_response_401;

const fostlib::urlhandler::view &fostlib::urlhandler::response_401 =c_response_401;
