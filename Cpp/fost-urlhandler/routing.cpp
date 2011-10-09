/*
    Copyright 2008-2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/urlhandler.hpp>
#include <fost/log>


namespace {
    void log_response(const fostlib::string &host, const fostlib::mime &body, int status_code) {
        fostlib::logging::info(host, status_code);
    }
}


bool fostlib::urlhandler::service( fostlib::http::server::request &req ) {
    fostlib::host h(req.data()->headers()["Host"].value());
    fostlib::string requested_host(h.name());

    fostlib::json host_config = c_hosts.value();
    if ( host_config.has_key(requested_host) ) {
        // Route the request to the right handler
        try {
            fostlib::json view_config = c_views.value();
            std::pair<fostlib::string, fostlib::json> view_fn = view::find_view(
                fostlib::coerce<fostlib::string>(host_config[requested_host]));

            std::pair<boost::shared_ptr<fostlib::mime>, int > resource(
                view::view_for(view_fn.first)(view_fn.second, req, h));
            log_response(requested_host, *resource.first, resource.second);
            req(*resource.first, resource.second);
        } catch ( fostlib::exceptions::exception &e ) {
            fostlib::text_body response(
                    fostlib::coerce<fostlib::string>(e),
                    fostlib::mime::mime_headers(), L"text/html" );
            fostlib::logging::error(fostlib::coerce<fostlib::string>(e), e.data());
            req( response, 501 );
        }
    } else {
        fostlib::text_body response(
                L"<html><body>No site found to service request</body></html>",
                fostlib::mime::mime_headers(), L"text/html" );
        fostlib::logging::error(requested_host, "Host configuration not found", host_config);
        req( response, 500 );
    }
    return true;
}
