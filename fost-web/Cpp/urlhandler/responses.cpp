/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>


const class response_404 : public urlhandler::view {
    public:
        response_404()
        : view("fost.response.404") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &,
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        L"<html><body>Resource not found</body></html>",
                        fostlib::mime::mime_headers(), L"text/html" ));
            return std::make_pair(response, 404);
        }
} c_response_404;

const urlhandler::view &urlhandler::response_404 =c_response_404;
