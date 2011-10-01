/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>


urlhandler::view::view(const fostlib::string &name) {
}

urlhandler::view::~view() {
}


const class error_404 : urlhandler::view {
    public:
        error_404()
        : view("404") {
        }

        void operator () (
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            fostlib::text_body response(
                    L"<html><body>Resource not found</body></html>",
                    fostlib::mime::mime_headers(), L"text/html" );
            req( response, 404 );
        }
} c_error_404;
