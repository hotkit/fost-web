/*
    Copyright 2011-2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/crypto>
#include <fost/push_back>
#include <fost/timestamp.hpp>
#include <fost/unicode>


const class response_static : public fostlib::urlhandler::view {
    public:
        response_static()
        : view("fost.static") {
        }

        static bool allow_delete(const fostlib::json &conf) {
            return conf.has_key("verbs") &&
                fostlib::coerce<fostlib::nullable<bool>>(conf["verbs"]["DELETE"]).value(false);
        }

        static fostlib::string etag(const boost::filesystem::wpath &filename) {
            fostlib::digester hash(fostlib::md5);
            hash << filename;
            return fostlib::coerce<fostlib::string>(fostlib::coerce<fostlib::hex_string>(hash.digest()));
        }

        template<typename A> inline
        static fostlib::string timestamp_to_string(const A &timestamp) {
            return fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::rfc1123_timestamp>(timestamp));
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
            if ( req.method() == "GET" || req.method() == "HEAD" ) {
                fostlib::mime::mime_headers headers;
                auto now = fostlib::timestamp::now();
                headers.set("Date", timestamp_to_string(now));
                static const fostlib::jcursor expires_path{"response", "headers", "expires"};
                if ( configuration.has_key(expires_path) ) {
                    auto expire_duration =
                        fostlib::coerce<fostlib::timediff>(configuration[expires_path]);
                    headers.set("Expires", timestamp_to_string(now + expire_duration));
                }
                fostlib::string validator = etag(filename);
                headers.set("ETag", "\"" + validator + "\"");
                headers.set("Content-Type", fostlib::urlhandler::mime_type(filename));
                if ( req.data()->headers().exists("If-None-Match") &&
                        (req.data()->headers()["If-None-Match"].value() == validator ||
                            req.data()->headers()["If-None-Match"].value() == "\"" + validator + "\"") ) {
                    boost::shared_ptr<fostlib::mime> response(
                            new fostlib::empty_mime(
                                fostlib::mime::mime_headers(), fostlib::urlhandler::mime_type(filename)));
                    response->headers() = headers;
                    return std::make_pair(response, 304);
                } else {
                    boost::shared_ptr<fostlib::mime> response(
                        new fostlib::file_body(filename, headers,
                            fostlib::urlhandler::mime_type(filename)));
                    return std::make_pair(response, 200);
                }

            } else if ( allow_delete(configuration) && req.method() == "DELETE" ) {
                boost::filesystem::remove(filename);
                boost::shared_ptr<fostlib::mime> response(
                        new fostlib::text_body(
                            L"<html><head><title>Resource deleted</title></head>"
                                L"<body><h1>Resource deleted</h1></body></html>",
                            fostlib::mime::mime_headers(), L"text/html" ));
                return std::make_pair(response, 200);
            } else {
                fostlib::json c405;
                fostlib::push_back(c405, "allow", "GET");
                fostlib::push_back(c405, "allow", "HEAD");
                if ( allow_delete(configuration) )
                    fostlib::push_back(c405, "allow", "DELETE");
                return fostlib::urlhandler::response_405(c405, path, req, host);
            }
        }
} c_response_static;

const fostlib::urlhandler::view &fostlib::urlhandler::static_server =c_response_static;
