/*
    Copyright 2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(response_303);


FSL_TEST_FUNCTION(no_location_config) {
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::response_303(
                    fostlib::json(), "/", req, fostlib::host()),
            fostlib::exceptions::not_implemented &);
}


FSL_TEST_FUNCTION(empty_location_config) {
    fostlib::json location;
    fostlib::insert(location, "location", "");
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::response_303(
                    location, "/", req, fostlib::host()),
            fostlib::exceptions::not_implemented &);
}


FSL_TEST_FUNCTION(with_location_config) {
    fostlib::json location;
    fostlib::insert(location, "location", "http://test.example.com/");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
            fostlib::urlhandler::response_303(
                    location, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 303);
    FSL_CHECK_EQ(
            response.first->headers()["Location"].value(),
            "http://test.example.com/");
}
