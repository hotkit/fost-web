/**
    Copyright 2011-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(routing);


namespace {
    void no_configuration_status(
            const fostlib::mime &, const fostlib::ascii_string &message) {
        FSL_CHECK_EQ(
                fostlib::ascii_string("500 Internal Server Error"), message);
    }
}

FSL_TEST_FUNCTION(no_configuration) {
    fostlib::http::server::request req(
            "GET", "/", std::make_unique<fostlib::binary_body>(),
            no_configuration_status);
    FSL_CHECK(fostlib::urlhandler::service(req));
}


namespace {
    void has_configuration_status(
            const fostlib::mime &, const fostlib::ascii_string &message) {
        FSL_CHECK_EQ(fostlib::ascii_string("404 Not Found"), message);
    }
}
FSL_TEST_FUNCTION(has_configuration) {
    fostlib::json configuration;
    fostlib::insert(configuration, "", "fost.response.404");
    const fostlib::setting<fostlib::json> host_config(
            "urlhandling/routing-tests.cpp/has_configuration", "webserver",
            "hosts", configuration);

    fostlib::http::server::request req(
            "GET", "/", std::make_unique<fostlib::binary_body>(),
            has_configuration_status);
    FSL_CHECK(fostlib::urlhandler::service(req));
}


namespace {
    void directory_error(
            const fostlib::mime &, const fostlib::ascii_string &message) {
        FSL_CHECK_EQ(fostlib::ascii_string("400 Bad Request"), message);
    }
}
FSL_TEST_FUNCTION(path_must_start_with_slash) {
    fostlib::http::server::request req(
            "GET", "path/to/file", std::make_unique<fostlib::binary_body>(),
            directory_error);
    FSL_CHECK(fostlib::urlhandler::service(req));
}
FSL_TEST_FUNCTION(slashdotdot_not_allowed) {
    fostlib::http::server::request req(
            "GET", "/..", std::make_unique<fostlib::binary_body>(),
            directory_error);
    FSL_CHECK(fostlib::urlhandler::service(req));
}
FSL_TEST_FUNCTION(dotdotslash_not_allowed) {
    fostlib::http::server::request req(
            "GET", "../", std::make_unique<fostlib::binary_body>(),
            directory_error);
    FSL_CHECK(fostlib::urlhandler::service(req));
}


FSL_TEST_FUNCTION(fostlib_exception_is_500) {
    fostlib::json configuration;
    fostlib::insert(configuration, "", "view", "test.throw");
    fostlib::insert(
            configuration, "", "configuration", "exception",
            "fostlib::exceptions::null");
    const fostlib::setting<fostlib::json> host_config{
            "urlhandling/routing-tests.cpp/fostlib_exception_is_500",
            "webserver", "hosts", configuration};
    fostlib::http::server::request req{
            "GET", "/", std::make_unique<fostlib::binary_body>(),
            [](auto const &, auto const &message) {
                FSL_CHECK_EQ(message, "500 Internal Server Error");
            }};
    FSL_CHECK(fostlib::urlhandler::service(req));
}


FSL_TEST_FUNCTION(fostlib_parse_error_is_400) {
    fostlib::json configuration;
    fostlib::insert(configuration, "", "view", "test.throw");
    fostlib::insert(
            configuration, "", "configuration", "exception",
            "fostlib::exceptions::parse_error");
    const fostlib::setting<fostlib::json> host_config{
            "urlhandling/routing-tests.cpp/fostlib_parse_error_is_400",
            "webserver", "hosts", configuration};
    fostlib::http::server::request req{
            "GET", "/", std::make_unique<fostlib::binary_body>(),
            [](auto const &, auto const &message) {
                FSL_CHECK_EQ(message, "400 Bad Request");
            }};
    FSL_CHECK(fostlib::urlhandler::service(req));
}
