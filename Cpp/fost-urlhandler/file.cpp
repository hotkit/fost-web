/**
    Copyright 2011-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/crypto>
#include <fost/timestamp.hpp>


namespace {
    fostlib::string etag(const fostlib::fs::path &filename) {
        fostlib::digester hash(fostlib::md5);
        hash << filename;
        return fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::hex_string>(hash.digest()));
    }

    template<typename A>
    inline fostlib::string timestamp_to_string(const A &timestamp) {
        return fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::rfc1123_timestamp>(timestamp));
    }
}


std::pair<boost::shared_ptr<fostlib::mime>, int> fostlib::urlhandler::serve_file(
        const fostlib::json &configuration,
        fostlib::http::server::request &req,
        const fostlib::fs::path &filename) {
    fostlib::mime::mime_headers headers;
    auto now = fostlib::timestamp::now();
    headers.set("Date", timestamp_to_string(now));
    static const fostlib::jcursor expires_path{"response", "headers",
                                               "expires"};
    if (configuration.has_key(expires_path)) {
        auto expire_duration =
                fostlib::coerce<fostlib::timediff>(configuration[expires_path]);
        headers.set("Expires", timestamp_to_string(now + expire_duration));
    }
    fostlib::string validator = etag(filename);
    headers.set("ETag", "\"" + validator + "\"");
    headers.set("Content-Type", fostlib::urlhandler::mime_type(filename));
    if (req.data()->headers().exists("If-None-Match")
        && (req.data()->headers()["If-None-Match"].value() == validator
            || req.data()->headers()["If-None-Match"].value()
                    == "\"" + validator + "\"")) {
        boost::shared_ptr<fostlib::mime> response(new fostlib::empty_mime(
                fostlib::mime::mime_headers(),
                fostlib::urlhandler::mime_type(filename)));
        response->headers() = headers;
        return std::make_pair(response, 304);
    } else {
        boost::shared_ptr<fostlib::mime> response(new fostlib::file_body(
                filename, headers, fostlib::urlhandler::mime_type(filename)));
        return std::make_pair(response, 200);
    }
}
