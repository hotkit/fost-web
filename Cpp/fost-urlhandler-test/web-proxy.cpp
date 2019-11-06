/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(web_proxy);


/// Connect to an upstream server over HTTP
FSL_TEST_FUNCTION(http) {}


/// Connect to an upstream server using TLS
FSL_TEST_FUNCTION(https) {}


/// Connect to an upstream server where the host header
/// has to be replaced to function correctly.
FSL_TEST_FUNCTION(host_replacement) {}
