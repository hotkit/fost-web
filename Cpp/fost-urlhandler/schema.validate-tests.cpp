/*
    Copyright 2018 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/push_back>
#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(schema_validation);


FSL_TEST_FUNCTION(no_schema_config) {
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::schema_validation(
                    fostlib::json(), "/", req, fostlib::host()),
            fostlib::exceptions::not_implemented &);
}

FSL_TEST_FUNCTION(invalid_json_body) {
    fostlib::json config;
    fostlib::insert(config, "schema", "type", "object");
    fostlib::insert(config, "schema", "name", "type", "string");

    fostlib::json body_data;
    fostlib::insert(body_data, "name", 2);
    fostlib::mime::mime_headers headers;
    auto body = std::make_unique<fostlib::binary_body>(
            fostlib::coerce<std::vector<unsigned char>>(
                    fostlib::utf8_string("{\"name\": 2}")));
    fostlib::http::server::request req{"GET", "/", std::move(body)};
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
            fostlib::urlhandler::schema_validation(
                    config, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 422);
}
