/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/crypto>
#include <fost/cache.hpp>


fostlib::string fostlib::hash(
        const fostlib::http::user_agent::request &request) {
    fostlib::digester md5(fostlib::md5);
    md5 << request.method() <<
        fostlib::coerce<fostlib::string>(request.address());
    return fostlib::coerce<fostlib::string>(
        fostlib::coerce<fostlib::hex_string>(md5.digest()));
}


fostlib::string fostlib::hash(
        const fostlib::http::user_agent::response &response) {
    fostlib::digester md5(fostlib::md5);
    md5 << response.method() <<
        fostlib::coerce<fostlib::string>(response.address());
    return fostlib::coerce<fostlib::string>(
        fostlib::coerce<fostlib::hex_string>(md5.digest()));
}


fostlib::string fostlib::variant(
        const fostlib::mime::mime_headers &headers) {
    if ( headers.exists("Vary") ) {
        return variant(headers, headers["Vary"].value());
    } else {
        return variant(headers, fostlib::string());
    }
}


fostlib::string fostlib::variant(
    const fostlib::mime::mime_headers &headers,
    const fostlib::string &vary
) {
    fostlib::digester md5(fostlib::md5);
    if ( !vary.empty() ) {
        const fostlib::string newline("\n"), separator(": ");
        fostlib::split_type names(fostlib::split(vary, ","));
        for ( auto name : names ) {
            if ( headers.exists(name) ) {
                md5 << name << separator <<
                    fostlib::coerce<fostlib::string>(headers[name]) << newline;
            } else {
                md5 << newline;
            }
        }
    }
    return fostlib::coerce<fostlib::string>(
        fostlib::coerce<fostlib::hex_string>(md5.digest()));
}

