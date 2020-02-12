/**
    Copyright 2019-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(catch_exception);


FSL_TEST_FUNCTION(try_works) {
    fostlib::json config;
    fostlib::insert(config, "view", "control.exception.catch");
    fostlib::insert(config, "configuration", "try", "fost.response.200");
    fostlib::http::server::request req;
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}


FSL_TEST_FUNCTION(catches_std_exception) {
    fostlib::json config;
    fostlib::insert(config, "view", "control.exception.catch");
    fostlib::insert(config, "configuration", "try", "view", "test.throw");
    fostlib::insert(
            config, "configuration", "try", "configuration", "exception",
            "std::logic_error");
    fostlib::insert(
            config, "configuration", "catch", "std::exception",
            "fost.response.200");
    fostlib::http::server::request req;
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}


FSL_TEST_FUNCTION(missing_catch_rethrows) {
    fostlib::json config;
    fostlib::insert(config, "view", "control.exception.catch");
    fostlib::insert(config, "configuration", "try", "view", "test.throw");
    fostlib::insert(
            config, "configuration", "try", "configuration", "exception",
            "std::logic_error");
    fostlib::http::server::request req;
    try {
        auto const [response, status] = fostlib::urlhandler::view::execute(
                config, "", req, fostlib::host{});
        FSL_CHECK(false);
    } catch (const std::logic_error &) {
        // OK, test passed
    }
}
