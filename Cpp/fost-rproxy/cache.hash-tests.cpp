/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/rproxy>
#include <fost/test>


FSL_TEST_SUITE(cache_hash);


FSL_TEST_FUNCTION(GET_simple_request) {
    FSL_CHECK_EQ(
        fostlib::hash(fostlib::http::user_agent::request(
            "GET", fostlib::url("http://localhost/"))),
        "ecea064e38ede52b47cab6fb652af22b");
}


FSL_TEST_FUNCTION(GET_simple_response) {
    FSL_CHECK_EQ(
        fostlib::hash(fostlib::http::user_agent::response(
            "GET", fostlib::url("http://localhost/"), 200,
            boost::shared_ptr<fostlib::binary_body>(new fostlib::binary_body))),
        "ecea064e38ede52b47cab6fb652af22b");
}


FSL_TEST_FUNCTION(empty_variant) {
    FSL_CHECK_EQ(
        fostlib::variant(fostlib::mime::mime_headers()),
        "d41d8cd98f00b204e9800998ecf8427e");
}


FSL_TEST_FUNCTION(variant_accept_language) {
    fostlib::mime::mime_headers headers;
    headers.set("Vary", "Accept-Language");
    headers.set("Accept-Language", "zn");
    FSL_CHECK_EQ(
        fostlib::variant(headers), "802c59148268bb50b07b57b5222a8d6d");
    headers.set("Accept", "text/html");
    FSL_CHECK_EQ(
        fostlib::variant(headers), "802c59148268bb50b07b57b5222a8d6d");
}


FSL_TEST_FUNCTION(variant_vary_passed_in) {
    fostlib::mime::mime_headers headers;
    headers.set("Vary", "Accept-Language");
    headers.set("Accept-Language", "zn");
    FSL_CHECK_EQ(
        fostlib::variant(headers, "Accept"),
        "68b329da9893e34099c7d8ad5cb9c940");
    headers.set("Accept", "text/html");
    FSL_CHECK_EQ(
        fostlib::variant(headers, "Accept"),
        "8664dfb4d398fe6ddc371a18a312cb9b");
}

