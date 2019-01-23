/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(throw_exception);


FSL_TEST_FUNCTION(not_implemented) {
    fostlib::json config;
    fostlib::http::server::request req;
    try {
        auto const response{fostlib::urlhandler::test_throw(
                config, "", req, fostlib::host{})};
        FSL_CHECK(false);
    } catch (fostlib::exceptions::not_implemented &) {}
}
