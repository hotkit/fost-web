/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/matcher.hpp>
#include <fost/push_back>
#include <fost/test>


FSL_TEST_SUITE(matcher);


FSL_TEST_FUNCTION(empty) {
    FSL_CHECK(not fostlib::matcher(fostlib::json(), ""));
}


FSL_TEST_FUNCTION(args_mismatch_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    auto m = fostlib::matcher(config, "");
    FSL_CHECK(not m);
}


FSL_TEST_FUNCTION(args_match_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    auto m = fostlib::matcher(config, "first");
    FSL_CHECK(m);
    FSL_CHECK_EQ(m.value().arguments.size(), 1u);
    FSL_CHECK_EQ(m.value().arguments[0], "first");
}


FSL_TEST_FUNCTION(args_match_2) {
    fostlib::json config;
    fostlib::push_back(config, "path", 2);
    fostlib::push_back(config, "path", 1);
    auto m = fostlib::matcher(config, "second/first/");
    FSL_CHECK(m);
    FSL_CHECK_EQ(m.value().arguments.size(), 2u);
    FSL_CHECK_EQ(m.value().arguments[0], "first");
    FSL_CHECK_EQ(m.value().arguments[1], "second");
}


FSL_TEST_FUNCTION(args_with_fixed_strings_match_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    fostlib::push_back(config, "path", "/foo");
    fostlib::push_back(config, "path", 2);
    auto m = fostlib::matcher(config, "first/foo/second/");
    FSL_CHECK(m);
    FSL_CHECK_EQ(m.value().arguments.size(), 2u);
    FSL_CHECK_EQ(m.value().arguments[0], "first");
    FSL_CHECK_EQ(m.value().arguments[1], "second");
}


FSL_TEST_FUNCTION(args_with_fixed_strings_mismatch_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    fostlib::push_back(config, "path", "/foo");
    fostlib::push_back(config, "path", 2);
    auto m = fostlib::matcher(config, "first/bar/second/");
    FSL_CHECK(not m);
}
