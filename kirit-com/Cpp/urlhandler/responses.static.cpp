/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>
#include <fost/unicode>


const class response_static : public urlhandler::view {
    public:
        response_static()
        : view("fost.static") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        fostlib::utf::load_file(L"kirit-com/Static/testserver/index.html"),
                        fostlib::mime::mime_headers(), L"text/html" ));
            return std::make_pair(response, 200);
        }
} c_response_static;

const urlhandler::view &urlhandler::static_server =c_response_static;
