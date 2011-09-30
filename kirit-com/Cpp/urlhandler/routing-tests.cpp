/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>
#include <fost/test>


FSL_TEST_SUITE(routing);


typedef boost::function<
    void (const fostlib::mime&, const fostlib::ascii_string&)> handler_fn;

namespace {
    void no_configuration_status(const fostlib::mime&, const fostlib::ascii_string &message) {
        FSL_CHECK_EQ(fostlib::ascii_string("500 Internal Server Error"), message);
    }
}

FSL_TEST_FUNCTION(no_configuration) {
    std::auto_ptr< fostlib::binary_body > headers(
        new fostlib::binary_body());
    fostlib::http::server::request req("GET", "/", headers,
        no_configuration_status);
    FSL_CHECK(urlhandler::service(req));
}

