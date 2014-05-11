#include <proxy/detail/exports.hpp>
#include <proxy/cache.hpp>
#include <fost/test>


FSL_TEST_SUITE(cache_hash);


FSL_TEST_FUNCTION(GET_simple) {
    FSL_CHECK_EQ(
        proxy::hash(fostlib::http::server::request("GET", "/")),
        "e0e39d220ff38421b6dd61a998975b28");
}

