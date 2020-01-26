/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */

#include <fost/test>
#include <fost/urlhandler>
#include <fost/push_back>
#include <fost/log>

FSL_TEST_SUITE(view_matcher);

namespace {
    const class test_echo_req_view : public fostlib::urlhandler::view {
      public:
        test_echo_req_view() : view("fost.view.test.echo") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            auto const req_body = fostlib::coerce<fostlib::string>(
                    fostlib::coerce<f5::u8view>(req.data()->data()));
            boost::shared_ptr<fostlib::mime> response{
                    new fostlib::text_body(req_body, req.headers())};
            return std::make_pair(response, 200);
        }
    } c_test_echo_req;

    fostlib::json configuration() {
        /*
        {
            "view": "fost.view.match",
            "configuration": {
                "match": [{
                    "path": ["/test", 1],
                    "execute": "fost.view.test.echo"
                },{
                    "path": ["/shop", 1, "/employee", 2],
                    "execute": "fost.view.test.echo"
                },{
                    "path": ["/shop", 1, "/employee", 2, "/address", 3],
                    "execute": "fost.view.test.echo"
                }],
                "no-match": "fost.response.404"
            }
        }
        */
        fostlib::json config{};
        fostlib::insert(config, "view", "fost.view.match");

        fostlib::json path1{};
        fostlib::push_back(path1, "path", "/test");
        fostlib::push_back(path1, "path", 1);
        fostlib::insert(path1, "execute", "fost.view.test.echo");
        fostlib::push_back(config, "configuration", "match", path1);

        fostlib::json path2{};
        fostlib::push_back(path2, "path", "/shop");
        fostlib::push_back(path2, "path", 1);
        fostlib::push_back(path2, "path", "/employee");
        fostlib::push_back(path2, "path", 2);
        fostlib::push_back(path2, "path", "/address");
        fostlib::push_back(path2, "path", 3);
        fostlib::insert(path2, "execute", "fost.view.test.echo");
        fostlib::push_back(config, "configuration", "match", path2);

        fostlib::json path3{};
        fostlib::push_back(path3, "path", "/shop");
        fostlib::push_back(path3, "path", 1);
        fostlib::push_back(path3, "path", "/employee");
        fostlib::push_back(path3, "path", 2);
        fostlib::insert(path3, "execute", "fost.view.test.echo");
        fostlib::push_back(config, "configuration", "match", path3);

        fostlib::insert(
                config, "configuration", "no-match", "fost.response.404");
        return config;
    }
}

FSL_TEST_FUNCTION(throw_exception_when_no_fallback_view_defined) {
    /*
    {
        "view": "fost.view.match",
        "configuration": {
            "match": [{}]
        }
    }
    */
    fostlib::http::server::request req("GET", "/");
    fostlib::json config{};
    fostlib::insert(config, "view", "fost.view.match");
    fostlib::json path1{};
    fostlib::push_back(config, "configuration", "match", path1);
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::view::execute(
                    config, "/test/fred", req, fostlib::host{}),
            fostlib::exceptions::not_implemented &);
}

FSL_TEST_FUNCTION(throw_exception_when_no_match_view_defined) {
    /*
    {
        "view": "fost.view.match",
        "configuration": {
        }
    }
    */
    fostlib::http::server::request req("GET", "/");
    fostlib::json config{};
    fostlib::insert(config, "view", "fost.view.match");
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::view::execute(
                    config, "/test/fred", req, fostlib::host{}),
            fostlib::exceptions::not_implemented &);
}

FSL_TEST_FUNCTION(view_matcher_can_match_one_path) {
    fostlib::http::server::request req("GET", "/");
    auto const config = configuration();
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "/test/fred", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);

    FSL_CHECK_EQ(response->headers().exists("__1"), true);
    FSL_CHECK_EQ(response->headers()["__1"].value(), "fred");
}

FSL_TEST_FUNCTION(view_matcher_can_match_two_path) {
    auto const config = configuration();
    fostlib::http::server::request req("GET", "/");
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "/shop/coffee/employee/ploy", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);

    FSL_CHECK_EQ(response->headers().exists("__1"), true);
    FSL_CHECK_EQ(response->headers()["__1"].value(), "coffee");
    FSL_CHECK_EQ(response->headers().exists("__2"), true);
    FSL_CHECK_EQ(response->headers()["__2"].value(), "ploy");
}

FSL_TEST_FUNCTION(view_matcher_match_the_longest_path) {
    auto const config = configuration();
    fostlib::http::server::request req("GET", "/");
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "/shop/coffee/employee/ploy/address/home", req,
            fostlib::host{});
    FSL_CHECK_EQ(status, 200);

    FSL_CHECK_EQ(response->headers().exists("__1"), true);
    FSL_CHECK_EQ(response->headers()["__1"].value(), "coffee");
    FSL_CHECK_EQ(response->headers().exists("__2"), true);
    FSL_CHECK_EQ(response->headers()["__2"].value(), "ploy");
    FSL_CHECK_EQ(response->headers().exists("__3"), true);
    FSL_CHECK_EQ(response->headers()["__3"].value(), "home");
}

FSL_TEST_FUNCTION(view_matcher_support_fallback) {
    auto const config = configuration();
    fostlib::http::server::request req("GET", "/some/random/url");
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "/some/random/url", req, fostlib::host{});
    FSL_CHECK_EQ(status, 404);
}