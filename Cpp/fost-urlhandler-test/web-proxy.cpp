/**
    Copyright 2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/insert>
#include <fost/urlhandler>
#include <fost/test>


namespace {
    auto proxy_config(f5::u8view const proxy, f5::u8view const upstream) {
        fostlib::json conf;
        insert(conf, "view", fostlib::string{"fost.proxy." + proxy});
        insert(conf, "configuration", "base", upstream);
        return conf;
    }
}


FSL_TEST_SUITE(web_proxy);


/// Connect to an upstream server over HTTP
FSL_TEST_FUNCTION(http) {
    fostlib::http::server::request req;
    req.headers().add("Host", "neverssl.com");
    auto const config = proxy_config("transparent", "http://neverssl.com/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/", req, fostlib::host{"neverssl.com"});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK(
            fostlib::string{response->body_as_string()}.find("NeverSSL")
            != fostlib::string::npos);
}


/// Connect to an upstream server using TLS
FSL_TEST_FUNCTION(https) {
    fostlib::http::server::request req;
    req.headers().add("Host", "sha256.badssl.com");
    auto const config =
            proxy_config("transparent", "https://sha256.badssl.com/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/", req, fostlib::host{"sha256.badssl.com"});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK(
            fostlib::string{response->body_as_string()}.find("sha256")
            != fostlib::string::npos);
}


/// Connect to an upstream server where the host header
/// has to be replaced to function correctly.
FSL_TEST_FUNCTION(host_replacement) {
    fostlib::http::server::request req;
    req.headers().add("Host", "sha512.badssl.com");
    auto const config = proxy_config("reverse", "https://sha256.badssl.com/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/", req, fostlib::host{"sha256.badssl.com"});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK(
            fostlib::string{response->body_as_string()}.find("sha256")
            != fostlib::string::npos);
}


/// A same-site redirect must have its `Location` fixed up so the next
/// request will also go through the proxy. This test assumes that the
/// proxy is used from the root of the local host web server.
FSL_TEST_FUNCTION(location_replacement) {
    fostlib::http::server::request req;
    req.headers().add("Host", "localhost");
    auto config = proxy_config("reverse", "https://kirit.com/");
    insert(config, "configuration", "Location", "http://localhost/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/_resources", req, fostlib::host{});
    FSL_CHECK_EQ(status, 302);
    FSL_CHECK_EQ(
            response->headers()["Location"].value(),
            "http://localhost/_resources/");
}


/// A same-site redirect must have its `Location` fixed up so the next
/// request will also go through the proxy. This test assumes that the
/// proxy is mounted at `/proxy/` on the local host.
FSL_TEST_FUNCTION(location_replacement_with_path) {
    fostlib::http::server::request req;
    req.headers().add("Host", "localhost");
    auto config = proxy_config("reverse", "https://kirit.com/");
    insert(config, "configuration", "Location", "http://localhost/proxy/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/_resources", req, fostlib::host{});
    FSL_CHECK_EQ(status, 302);
    FSL_CHECK_EQ(
            response->headers()["Location"].value(),
            "http://localhost/proxy/_resources/");
}


/// Check that the `Location` header is not replaced when the
/// configuration item for the proxy server location is not present.
FSL_TEST_FUNCTION(location_replacement_no_config) {
    fostlib::http::server::request req;
    req.headers().add("Host", "localhost");
    auto const config = proxy_config("reverse", "https://kirit.com/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/_resources", req, fostlib::host{});
    FSL_CHECK_EQ(status, 302);
    FSL_CHECK_EQ(response->headers()["Location"].value(), "/_resources/");
}


/// Check that the `Location` header is not replaced when the
/// location points to a host which is not the upstream one
FSL_TEST_FUNCTION(location_replacement_no_match) {
    fostlib::http::server::request req;
    req.headers().add("Host", "localhost");
    auto config = proxy_config("reverse", "https://www.kirit.com/");
    insert(config, "configuration", "Location", "http://localhost/proxy/");
    auto const [response, status] = fostlib::urlhandler::view::execute(
            config, "/", req, fostlib::host{});
    FSL_CHECK_EQ(status, 301);
    FSL_CHECK_EQ(response->headers()["Location"].value(), "https://kirit.com/");
}
