/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


namespace {
    fostlib::setting<fostlib::string> const c_mime_html{
            "responses.static.tests.cpp", "MIME", ".html", "text/html"};

    fostlib::fs::path const c_file_root{fostlib::unique_filename()};

    void setup() {
        fostlib::fs::create_directory(c_file_root);
        fostlib::utf::save_file(c_file_root / "index.html", "html");
    }
}


FSL_TEST_SUITE(static_files);


FSL_TEST_FUNCTION(file) {
    setup();
    fostlib::json conf;
    fostlib::insert(conf, "root", c_file_root);
    fostlib::http::server::request req("GET", "/index.html");
    auto [resp, status] = fostlib::urlhandler::static_server(
            conf, "index.html", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
}
