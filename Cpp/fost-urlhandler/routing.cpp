/*
    Copyright 2008-2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/log>


namespace {
    void log_response(
        const fostlib::string &host, const fostlib::mime &body, int status_code
    ) {
        fostlib::log::info()
            ("host", host)
            ("status code", status_code);
    }
}


bool fostlib::urlhandler::service( fostlib::http::server::request &req ) {
    // Before doing anything else run some sanity checks on the request
    if (
            req.file_spec().underlying().underlying()[0] != '/'
            || req.file_spec().underlying().underlying().find("/..") != std::string::npos
    ) {
        fostlib::text_body response(
                    fostlib::string("400 Bad Request\n"),
                    fostlib::mime::mime_headers(), L"text/plain" );
        req(response, 400);
        return true;
    }

    // Now process it
    fostlib::host h(req.data()->headers()["Host"].value());
    fostlib::string requested_host(h.name());

    fostlib::json host_config = c_hosts.value();
    if ( host_config.has_key(requested_host)
            || host_config.has_key(fostlib::string()) ) {
        // Route the request to the right handler
        try {
            fostlib::json view_config = c_views.value();
            std::pair<fostlib::string, fostlib::json> view_fn = view::find_view(
                fostlib::coerce<fostlib::string>(
                    host_config[host_config.has_key(requested_host) ?
                        requested_host : fostlib::string()]));

            fostlib::string path(fostlib::coerce<fostlib::string>(
                req.file_spec().underlying()).substr(1));

            std::pair<boost::shared_ptr<fostlib::mime>, int > resource(
                view::view_for(view_fn.first)(view_fn.second, path, req, h));
            log_response(requested_host, *resource.first, resource.second);
            req(*resource.first, resource.second);
        } catch ( fostlib::exceptions::exception &e ) {
            fostlib::text_body response(
                    fostlib::coerce<fostlib::string>(e),
                    fostlib::mime::mime_headers(), L"text/plain" );
            fostlib::log::error(fostlib::coerce<fostlib::string>(e), e.data());
            req( response, 501 );
        }
    } else {
        fostlib::text_body response(
                L"<html><body>No site found to service request</body></html>",
                fostlib::mime::mime_headers(), L"text/html" );
        fostlib::log::error()
            ("message", "Host configuration not found")
            ("requested host", requested_host)
            ("host configuration", host_config);
        req( response, 500 );
    }
    return true;
}
