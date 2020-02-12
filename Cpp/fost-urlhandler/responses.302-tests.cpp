/**
    Copyright 2012-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(response_302);


FSL_TEST_FUNCTION(no_location_config) {
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::view::execute(
                    fostlib::json{"fost.response.302"}, "/", req,
                    fostlib::host()),
            fostlib::exceptions::not_implemented &);
}


FSL_TEST_FUNCTION(empty_location_config) {
    fostlib::json location;
    fostlib::insert(location, "view", "fost.response.302");
    fostlib::insert(location, "configuration", "location", "");
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::view::execute(
                    location, "/", req, fostlib::host()),
            fostlib::exceptions::not_implemented &);
}


FSL_TEST_FUNCTION(with_location_config) {
    fostlib::json location;
    fostlib::insert(location, "view", "fost.response.302");
    fostlib::insert(
            location, "configuration", "location", "http://test.example.com/");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
            fostlib::urlhandler::view::execute(
                    location, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 302);
    FSL_CHECK_EQ(
            response.first->headers()["Location"].value(),
            "http://test.example.com/");
}
