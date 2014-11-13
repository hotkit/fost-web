/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/rproxy>
#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/test>


FSL_TEST_SUITE(config);


FSL_TEST_FUNCTION(can_get_page) {
    fostlib::http::server::request req("GET", "/");
    FSL_CHECK_EXCEPTION(
        fostlib::view::c_config(fostlib::json(), "/", req, fostlib::host()),
        fostlib::exceptions::not_implemented &);
}

