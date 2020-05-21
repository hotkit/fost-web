/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/push_back>
#include <fost/test>


FSL_TEST_SUITE(middleware_replace);


FSL_TEST_FUNCTION(nop) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "fost.response.200");
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
}


FSL_TEST_FUNCTION(simple) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "fost.response.200");
    fostlib::insert(config, "configuration", "replace", "OK", "Very OK");
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
            response.first->body_as_string(),
            "<html><head><title>Very OK</title></head><body><h1>Very "
            "OK</h1></body></html>");
}


FSL_TEST_FUNCTION(setting) {
    fostlib::setting<fostlib::string> const s{
            "middleware.replace.tests.cpp", "Test middleware", "Replacement",
            "Very OK", true};
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "fost.response.200");
    fostlib::push_back(config, "configuration", "replace", "OK", "setting");
    fostlib::push_back(config, "configuration", "replace", "OK", s.section());
    fostlib::push_back(config, "configuration", "replace", "OK", s.name());
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
            response.first->body_as_string(),
            "<html><head><title>Very OK</title></head><body><h1>Very "
            "OK</h1></body></html>");
}


FSL_TEST_FUNCTION(setting_with_default) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "fost.response.200");
    fostlib::push_back(config, "configuration", "replace", "OK", "setting");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Test middleware");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Not a setting value");
    fostlib::push_back(config, "configuration", "replace", "OK", "Default");
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
            response.first->body_as_string(),
            "<html><head><title>Default</title></head><body><h1>Default</h1></"
            "body></html>");
}


FSL_TEST_FUNCTION(setting_with_default_object) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "fost.response.200");
    fostlib::push_back(config, "configuration", "replace", "OK", "setting");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Test middleware");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Not a setting value");
    fostlib::jcursor{"configuration", "replace", "OK"}.push_back(
            config, fostlib::json::object_t{});
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
            response.first->body_as_string(),
            "<html><head><title>{}</title></head><body><h1>{}</h1></body></"
            "html>");
}


// test do replace on text/html file already test by other above test with view
// fost.response.200


namespace {
    const class response_font : public fostlib::urlhandler::view {
      public:
        response_font() : view("test.response.font") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &,
                fostlib::http::server::request &req,
                const fostlib::host &) const {
            boost::shared_ptr<fostlib::mime> response;
            response.reset(new fostlib::text_body(
                    "A B C D E F G OK", fostlib::mime::mime_headers(),
                    "font/ttf"));
            return std::make_pair(response, 200);
        }
    } c_response_font;
}


FSL_TEST_FUNCTION(should_do_replace_only_text_and_javascript_file) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "test.response.font");
    fostlib::push_back(config, "configuration", "replace", "OK", "setting");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Test middleware");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Not a setting value");
    fostlib::push_back(config, "configuration", "replace", "OK", "Default");
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(response.first->body_as_string(), "A B C D E F G OK");
}


namespace {
    const class response_plain : public fostlib::urlhandler::view {
      public:
        response_plain() : view("test.response.plain") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &,
                fostlib::http::server::request &req,
                const fostlib::host &) const {
            boost::shared_ptr<fostlib::mime> response;
            response.reset(new fostlib::text_body(
                    "<html><head><title>OK</title></head>"
                    "<body><h1>OK</h1></body></html>",
                    fostlib::mime::mime_headers(), "text/plain"));
            return std::make_pair(response, 200);
        }
    } c_response_plain;
}


FSL_TEST_FUNCTION(should_do_replace_text_file) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(config, "configuration", "view", "test.response.plain");
    fostlib::push_back(config, "configuration", "replace", "OK", "setting");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Test middleware");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Not a setting value");
    fostlib::push_back(config, "configuration", "replace", "OK", "Default");
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
            response.first->body_as_string(),
            "<html><head><title>Default</title></head><body><h1>Default</h1></"
            "body></html>");
}


namespace {
    const class response_javascript : public fostlib::urlhandler::view {
      public:
        response_javascript() : view("test.response.javascript") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &,
                fostlib::http::server::request &req,
                const fostlib::host &) const {
            boost::shared_ptr<fostlib::mime> response;
            response.reset(new fostlib::text_body(
                    "<script>console.log('OK')</script>",
                    fostlib::mime::mime_headers(), "application/javascript"));
            return std::make_pair(response, 200);
        }
    } c_response_javascript;
}


FSL_TEST_FUNCTION(should_do_replace_javascript_file) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.middleware.replace");
    fostlib::insert(
            config, "configuration", "view", "test.response.javascript");
    fostlib::push_back(config, "configuration", "replace", "OK", "setting");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Test middleware");
    fostlib::push_back(
            config, "configuration", "replace", "OK", "Not a setting value");
    fostlib::push_back(config, "configuration", "replace", "OK", "Default");
    fostlib::http::server::request req;
    auto response = fostlib::urlhandler::view::execute(
            config, "", req, fostlib::host{});
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
            response.first->body_as_string(),
            "<script>console.log('Default')</script>");
}
