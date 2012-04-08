/*
    Copyright 2011-2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/unicode>


const class response_static : public fostlib::urlhandler::view {
    public:
        response_static()
        : view("fost.static") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &configuration,
            const fostlib::string &path,
            fostlib::http::server::request &req,
            const fostlib::host &host
        ) const {
            boost::filesystem::wpath root(
                fostlib::coerce<boost::filesystem::wpath>(configuration["root"]));
            boost::filesystem::wpath filename = root /
                fostlib::coerce<boost::filesystem::wpath>(path);
            if ( boost::filesystem::is_directory(filename) )
                filename /= L"index.html";
            if ( !boost::filesystem::exists(filename) )
                return fostlib::urlhandler::response_404(fostlib::json(), path, req, host);
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::file_body(filename, fostlib::mime::mime_headers(),
                        fostlib::urlhandler::mime_type(filename)));
            return std::make_pair(response, 200);
        }
} c_response_static;

const fostlib::urlhandler::view &fostlib::urlhandler::static_server =c_response_static;
