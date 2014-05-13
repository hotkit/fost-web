#include <proxy/detail/exports.hpp>
#include <proxy/cache.hpp>
#include <fost/test>


FSL_TEST_SUITE(cache_hash);


FSL_TEST_FUNCTION(GET_simple) {
    FSL_CHECK_EQ(
        proxy::hash(fostlib::http::server::request("GET", "/")),
        "e0e39d220ff38421b6dd61a998975b28");
}


FSL_TEST_FUNCTION(empty_variant) {
    FSL_CHECK_EQ(
        proxy::variant(fostlib::mime::mime_headers()),
        "d41d8cd98f00b204e9800998ecf8427e");
}


FSL_TEST_FUNCTION(variant_accept_language) {
    fostlib::mime::mime_headers headers;
    headers.set("Vary", "Accept-Language");
    headers.set("Accept-Language", "zn");
    FSL_CHECK_EQ(
        proxy::variant(headers), "802c59148268bb50b07b57b5222a8d6d");
    headers.set("Accept", "text/html");
    FSL_CHECK_EQ(
        proxy::variant(headers), "802c59148268bb50b07b57b5222a8d6d");
}

