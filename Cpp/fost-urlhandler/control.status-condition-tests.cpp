/**
    Copyright 2018-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(control_status_condition);


FSL_TEST_FUNCTION(execute_then_if_less_than_400) {
    fostlib::json config;
    fostlib::insert(config, "if", "fost.response.200");
    fostlib::insert(config, "then", "fost.response.404");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::control_status_condition(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 404);
}

FSL_TEST_FUNCTION(return_if_default) {
    fostlib::json config;
    fostlib::insert(config, "if", "fost.response.200");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::control_status_condition(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 200);
}


FSL_TEST_FUNCTION(return_specific_then) {
    fostlib::json config;
    fostlib::insert(config, "if", "fost.response.200");
    fostlib::insert(config, "then.200", "fost.response.404");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::control_status_condition(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 404);
}

FSL_TEST_FUNCTION(execute_else_if_more_than_400) {
    fostlib::json config;
    fostlib::insert(config, "if", "fost.response.404");
    fostlib::insert(config, "else", "fost.response.200");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::control_status_condition(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 200);
}

FSL_TEST_FUNCTION(return_response_if_more_than_400_without_else) {
    fostlib::json config;
    fostlib::insert(config, "if", "fost.response.404");
    fostlib::http::server::request req;
    std::pair<boost::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::control_status_condition(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 404);
}
