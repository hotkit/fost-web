#include <proxy/proxy-lib>
#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/test>


FSL_TEST_SUITE(proxy);


FSL_TEST_FUNCTION(can_get_page) {
    fostlib::http::server::request req(
        "GET", "/images/league_images/la_liga.jpg");
    std::pair<boost::shared_ptr<fostlib::mime>, int > response =
        proxy::view::c_proxy(
            fostlib::json(), "/images/league_images/la_liga.jpg",
            req, fostlib::host());
    FSL_CHECK_EQ(response.second, 200);
    FSL_CHECK_EQ(
        response.first->headers()["content-type"].value(),
        "image/jpeg");
}

