/**
    Copyright 2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */

#include <fost/test>
#include <fost/urlhandler>

FSL_TEST_SUITE(fost_req_id);

namespace {
    void check_fost_req_id(
            const fostlib::mime &req, const fostlib::ascii_string &message) {
        FSL_CHECK(req.headers().exists("Fost-Request-ID"));
    }
}

FSL_TEST_FUNCTION(fost_req_id_in_header) {
    fostlib::json configuration;
    fostlib::insert(configuration, "", "fost.response.200");
    const fostlib::setting<fostlib::json> host_config(
            "urlhandling/view.request-id.tests.cpp", "webserver", "hosts",
            configuration);

    fostlib::http::server::request req(
            "GET", "/", std::make_unique<fostlib::binary_body>(),
            check_fost_req_id);
    FSL_CHECK(fostlib::urlhandler::service(req));
}
