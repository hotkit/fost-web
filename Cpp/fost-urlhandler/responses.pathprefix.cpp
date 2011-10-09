/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/urlhandler.hpp>


const class pathprefix : public fostlib::urlhandler::view {
    public:
        pathprefix()
        : view("fost.view.pathprefix") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &configuration,
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            fostlib::string path;
            if (configuration.has_key("path"))
                path = fostlib::coerce<fostlib::string>(configuration["path"]);
            else
                path = fostlib::coerce<fostlib::string>(
                    fostlib::coerce<boost::filesystem::wpath>(
                        req.file_spec()));

            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        L"<html><body><h1>Path prefix</h1>" +
                            path + L"</body></html>",
                        fostlib::mime::mime_headers(), L"text/html" ));
            return std::make_pair(response, 200);
        }
} c_pathprefix;

const fostlib::urlhandler::view &fostlib::urlhandler::view_pathprefix = c_pathprefix;
