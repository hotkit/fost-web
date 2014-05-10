#include <fost/http>
#include <fost/test>
#include <proxy/webserver.hpp>


FSL_TEST_SUITE(webserver);


FSL_TEST_FUNCTION(starts_and_stops) {
    fostlib::http::user_agent ua;

    FSL_CHECK_EXCEPTION(
        ua.get(fostlib::url("http://localhost:2555")),
        fostlib::exceptions::connect_failure&);
    proxy::start("/");
    sleep(1);
    FSL_CHECK_NOTHROW(ua.get(fostlib::url("http://localhost:2555")));
    proxy::stop();
    proxy::wait();
    FSL_CHECK_EXCEPTION(
        ua.get(fostlib::url("http://localhost:2555")),
        fostlib::exceptions::connect_failure&);
}

