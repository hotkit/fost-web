/*
    Copyright 2012-2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/push_back>
#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(response_405);


FSL_TEST_FUNCTION(no_allows_config) {
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(fostlib::urlhandler::response_405(
            fostlib::json(), "/", req, fostlib::host()),
        fostlib::exceptions::not_implemented&);
}


FSL_TEST_FUNCTION(empty_allows_config) {
    fostlib::json allows;
    fostlib::insert(allows, "allow", fostlib::json::array_t());
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
        fostlib::urlhandler::response_405(
            allows, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 405);
    FSL_CHECK(response.first->headers().exists("Allow"));
    FSL_CHECK_EQ(response.first->headers()["Allow"].value(), "");
}


FSL_TEST_FUNCTION(one_allows_config) {
    fostlib::json allows;
    fostlib::push_back(allows, "allow", "GET");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
        fostlib::urlhandler::response_405(
            allows, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 405);
    FSL_CHECK_EQ(response.first->headers()["Allow"].value(), "GET");
}


FSL_TEST_FUNCTION(multiple_allows_config) {
    fostlib::json allows;
    fostlib::push_back(allows, "allow", "GET");
    fostlib::push_back(allows, "allow", "POST");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
        fostlib::urlhandler::response_405(
            allows, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 405);
    FSL_CHECK_EQ(response.first->headers()["Allow"].value(), "GET, POST");
}

