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
    const class check_fost_view_match_set_header :
    public fostlib::urlhandler::view {
      public:
        check_fost_view_match_set_header()
        : view("fost.view.test.match_set_header") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            FSL_CHECK_EQ(req.headers().exists("__1"), true);
            boost::shared_ptr<fostlib::mime> response{
                    new fostlib::text_body(L"OK")};
            return std::make_pair(response, 200);
        }
    } c_check_fost_view_match_set_header;
}

FSL_TEST_FUNCTION(view_matcher_call_correct_view) {
    /*
    {
        "view": "fost.view.match",
        "configuration": {
                "match": [{
                "path": ["/test", 1],
                "execute": "fost.response.200"
        },{ // fallback
                "path": [],
                "execute": "fost.response.404"
        }]
        }
    }
    */
    fostlib::http::server::request req("GET", "/test/fred");
    fostlib::json config{};
    fostlib::insert(config, "view", "fost.view.match");

    fostlib::json path1{};
    fostlib::push_back(path1, "path", "/test");
    fostlib::push_back(path1, "path", 1);
    fostlib::insert(path1, "execute", "fost.view.test.match_set_header");
    fostlib::push_back(config, "configuration", "match", path1);
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "/test/fred", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}
