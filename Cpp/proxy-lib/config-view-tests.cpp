#include <proxy/proxy-lib>
#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/test>


FSL_TEST_SUITE(config);


FSL_TEST_FUNCTION(can_get_page) {
    fostlib::http::server::request req("GET", "/");
    FSL_CHECK_EXCEPTION(
        proxy::view::c_config(fostlib::json(), "/", req, fostlib::host()),
        fostlib::exceptions::not_implemented &);
}

