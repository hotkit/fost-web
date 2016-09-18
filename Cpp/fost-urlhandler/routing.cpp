/*
    Copyright 2008-2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/log>


std::pair<boost::shared_ptr<fostlib::mime>, int> fostlib::urlhandler::router(
    const fostlib::host &requested_host,
    const fostlib::string &viewname,
    fostlib::http::server::request &req
) {
    std::pair<fostlib::string, fostlib::json> view_fn = view::find_view(viewname);

    fostlib::string path(fostlib::coerce<fostlib::string>(
        req.file_spec().underlying()).substr(1));

    return view::view_for(view_fn.first)(view_fn.second, path, req, requested_host);
}


bool fostlib::urlhandler::service( fostlib::http::server::request &req ) {
    // Before doing anything else run some sanity checks on the request
    if (
            req.file_spec().underlying().underlying()[0] != '/'
            || req.file_spec().underlying().underlying().find("/..") != std::string::npos
    ) {
        fostlib::log::error(c_fost_web_urlhandler)
            ("", "fostlib::urlhandler::service -- Bad request received, could not be parsed");
        fostlib::text_body response(
                    fostlib::string("400 Bad Request\n"),
                    fostlib::mime::mime_headers(), L"text/plain" );
        req(response, 400);
        return true;
    }

    // Now process it
    fostlib::string hostname(req.data()->headers()["Host"].value());

    fostlib::json host_config = c_hosts.value();
    if ( host_config.has_key(hostname)
            || host_config.has_key(fostlib::string()) ) {
        // Route the request to the right handler
        try {
            fostlib::json view_name = host_config[
                host_config.has_key(hostname) ? hostname : fostlib::string()];
            std::pair<boost::shared_ptr<fostlib::mime>, int > resource(
                router(host(hostname), fostlib::coerce<fostlib::string>(view_name), req));
            req(*resource.first, resource.second);
        } catch ( fostlib::exceptions::exception &e ) {
            fostlib::log::error(c_fost_web_urlhandler)
                ("", "fostlib::urlhandler::service -- fostlib::exceptions::exception")
                ("exception", coerce<json>(e));
            fostlib::text_body response(
                    utf8_string("<html><body>An error occurred in the request</body></html>"),
                    fostlib::mime::mime_headers(), L"text/html" );
            req(response, 500);
        } catch ( std::exception &e ) {
            fostlib::log::error(c_fost_web_urlhandler)
                ("", "fostlib::urlhandler::service -- std::exception")
                ("exception", "message", e.what())
                ("exception", "type", typeid(e).name());
            fostlib::text_body response(
                utf8_string("<html><body>An error occurred in the request</body></html>"),
                fostlib::mime::mime_headers(), L"text/html");
            req(response, 500);
        }
    } else {
        fostlib::log::warning(c_fost_web_urlhandler)
            ("", "fostlib::urlhandler::service -- No configured web site")
            ("hostname", hostname)
            ("config", host_config);
        fostlib::text_body response(
                L"<html><body>No site found to service request</body></html>",
                fostlib::mime::mime_headers(), L"text/html" );
        req( response, 500 );
    }
    return true;
}

