/*
    Copyright 2008-2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>
#include <fost/log>


namespace {
    const fostlib::setting< fostlib::json > c_hosts(
        "urlhandler/routing.cpp",
        "webserver", "hosts",
        fostlib::json::object_t(), true);


    void log_response(const fostlib::string &host, const fostlib::mime &body, int status_code) {
        fostlib::logging::info(host, status_code);
    }
}


bool urlhandler::service( fostlib::http::server::request &req ) {
    fostlib::host h(req.data()->headers()["Host"].value());
    fostlib::string requested_host(h.name());
    fostlib::logging::debug(requested_host);

    fostlib::json host_config = c_hosts.value();
    if ( host_config.has_key(requested_host) ) {
        // Route the request to the right handler
        fostlib::text_body response(
                L"<html><body>Handler not implemented</body></html>",
                fostlib::mime::mime_headers(), L"text/html" );
        log_response(requested_host, response, 501);
        req( response, 501 );
    } else {
        fostlib::text_body response(
                L"<html><body>No site found to service request</body></html>",
                fostlib::mime::mime_headers(), L"text/html" );
        fostlib::logging::error(requested_host, "Host configuration not found", 500);
        req( response, 500 );
    }
    return true;
}
