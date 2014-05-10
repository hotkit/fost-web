#include <proxy/proxy-lib>
#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/test>


FSL_TEST_SUITE(proxy);


FSL_TEST_FUNCTION(can_get_page) {
    fostlib::http::server::request req("GET", "/GPLed%20TLA%20FAQ");
    std::pair<boost::shared_ptr<fostlib::mime>, int > response =
        proxy::view::c_proxy(
            fostlib::json(), "/GPLed TLA FAQ", req, fostlib::host());
}

