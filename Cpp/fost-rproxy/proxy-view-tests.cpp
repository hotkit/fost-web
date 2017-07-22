/*
    Copyright 2014-2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/rproxy>
#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/test>


FSL_TEST_SUITE(proxy);


FSL_TEST_FUNCTION(can_get_page) {
    // TODO This test can't be a unit test as it access the net
    fostlib::json config;
    fostlib::insert(config, "origin", "https://kirit.com/");
    fostlib::http::server::request req(
        "GET", "/_resources/buttercup/logo.png");
    std::pair<boost::shared_ptr<fostlib::mime>, int > response;
    FSL_CHECK_NOTHROW(
        response = fostlib::view::c_proxy(
            config, "/_resources/buttercup/logo.png",
            req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
        response.first->headers()["content-type"].value(),
        "image/png");
}

