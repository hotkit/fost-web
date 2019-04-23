/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


namespace {


    /// Fetch the response body as text
    fostlib::string content(const fostlib::mime &body) {
        fostlib::string text;
        for (auto part : body) {
            text += std::string{reinterpret_cast<char const *>(part.first),
                                reinterpret_cast<char const *>(part.second)};
        }
        return text;
    }


}

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


FSL_TEST_FUNCTION(generic_response) {
    fostlib::http::server::request req("GET", "/");
    auto [response, status] = fostlib::urlhandler::view::execute(
            fostlib::json{"fost.response"}, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}

FSL_TEST_FUNCTION(generic_response_can_config_status_code) {
    fostlib::http::server::request req("GET", "/");
    fostlib::json config{};
    fostlib::insert(config, "view", "fost.response");
    fostlib::insert(config, "configuration", "status", 201);
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 201);
    FSL_CHECK_EQ(response->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK_EQ(
            content(*response),
            "<html><head><title>OK</title></head><body><h1>OK</h1></body></"
            "html>");
}


FSL_TEST_FUNCTION(generic_response_can_config_message) {
    fostlib::http::server::request req("GET", "/");
    fostlib::json config{};
    fostlib::insert(config, "view", "fost.response");
    fostlib::insert(config, "configuration", "message", "Custom message");
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK_EQ(
            content(*response),
            "<html><head><title>Custom message</title></head><body><h1>Custom "
            "message</h1></body></html>");
    FSL_CHECK_EQ(status, 200);
}


FSL_TEST_FUNCTION(generic_response_can_config_json) {
    fostlib::http::server::request req("GET", "/");
    fostlib::json config{};
    fostlib::insert(config, "view", "fost.response");
    fostlib::insert(
            config, "configuration", "json", "message", "custom message");
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(
            response->headers()["Content-Type"].value(), "application/json");
    fostlib::json expected_body{};
    fostlib::insert(expected_body, "message", "custom message");
    FSL_CHECK_EQ(fostlib::json::parse(content(*response)), expected_body);
    FSL_CHECK_EQ(status, 200);
}
