/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(generic_responses);


FSL_TEST_FUNCTION(404) {
    fostlib::http::server::request req("GET", "/index.html");
    auto [response, status] = fostlib::urlhandler::view::execute(
            fostlib::json{"fost.response.404"}, "index.html", req,
            fostlib::host{});
    FSL_CHECK_EQ(status, 404);
}


FSL_TEST_FUNCTION(500) {
    fostlib::http::server::request req("GET", "/");
    auto [response, status] = fostlib::urlhandler::view::execute(
            fostlib::json{"fost.response.500"}, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 500);
}


FSL_TEST_FUNCTION(200) {
    fostlib::http::server::request req("GET", "/");
    auto [response, status] = fostlib::urlhandler::view::execute(
            fostlib::json{"fost.response"}, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}
