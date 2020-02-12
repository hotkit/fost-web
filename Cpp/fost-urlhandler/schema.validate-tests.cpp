/**
    Copyright 2018-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/push_back>
#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(schema_validation);


FSL_TEST_FUNCTION(no_schema_config) {
    fostlib::http::server::request req;
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::view::execute(
                    fostlib::json{"fost.schema.validate"}, "/", req,
                    fostlib::host()),
            fostlib::exceptions::not_implemented &);
}

FSL_TEST_FUNCTION(invalid_json_body) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.schema.validate");
    fostlib::insert(config, "configuration", "schema", "type", "object");
    fostlib::insert(
            config, "configuration", "schema", "properties", "is_valid", "type",
            "boolean");
    fostlib::mime::mime_headers headers;
    auto body = std::make_unique<fostlib::binary_body>(
            fostlib::coerce<std::vector<unsigned char>>(
                    fostlib::utf8_string("{\"is_valid\": \"Hello\"}")));
    fostlib::http::server::request req{"GET", "/", std::move(body)};
    auto response(fostlib::urlhandler::view::execute(
            config, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 422);
}

FSL_TEST_FUNCTION(valid_json_body) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.schema.validate");
    fostlib::insert(config, "configuration", "schema", "type", "object");
    fostlib::insert(
            config, "configuration", "schema", "properties", "is_valid", "type",
            "boolean");
    fostlib::insert(config, "configuration", "valid", "fost.response.200");
    fostlib::mime::mime_headers headers;
    auto body = std::make_unique<fostlib::binary_body>(
            fostlib::coerce<std::vector<unsigned char>>(
                    fostlib::utf8_string("{\"is_valid\": true}")));
    fostlib::http::server::request req{"GET", "/", std::move(body)};
    auto response(fostlib::urlhandler::view::execute(
            config, "/", req, fostlib::host()));
    FSL_CHECK_EQ(response.second, 200);
}
