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
    auto const [response, status] = fostlib::urlhandler::control_exception_catch(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}
