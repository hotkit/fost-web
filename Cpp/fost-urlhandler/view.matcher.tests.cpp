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
    void check_fost_req_id(
            const fostlib::mime &req, const fostlib::ascii_string &message) {
        FSL_CHECK(req.headers().exists("Fost-Request-ID"));
    }
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
    fostlib::insert(path1, "execute", "fost.response.200");
    fostlib::push_back(config, "configuration", "match", path1);
    fostlib::log::debug(fostlib::c_fost_web_urlhandler)("", config);
    auto [response, status] = fostlib::urlhandler::view::execute(
            config, "/test/fred", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
}
