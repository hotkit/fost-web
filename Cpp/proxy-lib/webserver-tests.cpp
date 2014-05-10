#include <fost/test>
#include <proxy/webserver.hpp>


FSL_TEST_SUITE(webserver);


FSL_TEST_FUNCTION(starts_and_stops) {
    proxy::start("/");
    proxy::stop();
    proxy::wait();
}

