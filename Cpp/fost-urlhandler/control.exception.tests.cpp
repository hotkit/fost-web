/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(catch_exception);


FSL_TEST_FUNCTION(try_works) {
    fostlib::json config;
    fostlib::insert(config, "try", "fost.response.200");
    fostlib::http::server::request req;
    auto const [response, status] =
            fostlib::urlhandler::control_exception_catch(
                    config, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}


FSL_TEST_FUNCTION(catches_std_exception) {
    fostlib::json config;
    fostlib::insert(config, "try", "view", "test.throw");
    fostlib::insert(
            config, "try", "configuration", "exception", "std::logic_error");
    fostlib::insert(config, "catch", "std::exception", "fost.response.200");
    fostlib::http::server::request req;
    auto const [response, status] =
            fostlib::urlhandler::control_exception_catch(
                    config, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}


FSL_TEST_FUNCTION(missing_catch_rethrows) {
    fostlib::json config;
    fostlib::insert(config, "try", "view", "test.throw");
    fostlib::insert(
            config, "try", "configuration", "exception", "std::logic_error");
    fostlib::http::server::request req;
    try {
        auto const [response, status] =
                fostlib::urlhandler::control_exception_catch(
                        config, "", req, fostlib::host{});
        FSL_CHECK(false);
    } catch (const std::logic_error &) {
        // OK, test passed
    }
}
