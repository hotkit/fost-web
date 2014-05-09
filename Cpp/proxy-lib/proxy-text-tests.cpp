#include <proxy/proxy-lib>
#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/test>


FSL_TEST_SUITE(proxy_lib);


FSL_TEST_FUNCTION(can_see_config) {
    fostlib::http::server::request req("GET", "/_config");
}

