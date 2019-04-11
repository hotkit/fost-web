/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


namespace {


    /// Set up some proper MIME content types
    fostlib::setting<fostlib::string> const c_mime_html{
            "responses.static.tests.cpp", "MIME", ".html", "text/html"};

    /// We need to write the files we're going to test against
    /// to a suitable location
    fostlib::fs::path const c_file_root{fostlib::unique_filename()};


    /// Set up the tests. Most of this only needs to be run once,
    /// but it doesn't matter if it runs multiple times
    fostlib::json setup() {
        fostlib::fs::create_directory(c_file_root);
        fostlib::fs::create_directory(c_file_root / "empty");
        fostlib::utf::save_file(c_file_root / "index.html", "html");
        fostlib::utf::save_file(c_file_root / "index.txt", "text");
        fostlib::json conf;
        fostlib::insert(conf, "root", c_file_root);
        return conf;
    }


    /// Fetch the response body as text
    fostlib::string content(const fostlib::mime &body) {
        fostlib::string text;
        for (auto part : body) {
            text += std::string{reinterpret_cast<char const *>(part.first),
                                reinterpret_cast<char const *>(part.second)};
        }
        return text;
    }


}


FSL_TEST_SUITE(static_files);


/// Serve a single specified file
FSL_TEST_FUNCTION(file) {
    auto conf = setup();
    fostlib::http::server::request req("GET", "/index.html");
    auto [resp, status] = fostlib::urlhandler::static_server(
            conf, "index.html", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK_EQ(content(*resp), "html");
}


/// Serve the default file in the directory
FSL_TEST_FUNCTION(default_file) {
    auto conf = setup();
    fostlib::http::server::request req("GET", "/");
    auto [resp, status] =
            fostlib::urlhandler::static_server(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK_EQ(content(*resp), "html");
}


/// Serve a custom index file from the directory
FSL_TEST_FUNCTION(custom_default_file) {
    auto conf = setup();
    fostlib::insert(conf, "index", "index.txt");
    fostlib::http::server::request req("GET", "/");
    auto [resp, status] =
            fostlib::urlhandler::static_server(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/plain");
    FSL_CHECK_EQ(content(*resp), "text");
}


/// Fail to serve a default files from the directory
FSL_TEST_FUNCTION(default_file_not_found) {
    auto conf = setup();
    fostlib::http::server::request req("GET", "/empty/");
    auto [resp, status] = fostlib::urlhandler::static_server(
            conf, "empty/", req, fostlib::host{});
    FSL_CHECK_EQ(status, 403);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
}
FSL_TEST_FUNCTION(custom_default_file_not_found) {
    auto conf = setup();
    fostlib::insert(conf, "index", "non-existant.txt");
    fostlib::http::server::request req("GET", "/");
    auto [resp, status] =
            fostlib::urlhandler::static_server(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 403);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
}


/// Directory redirects without trailing slash
FSL_TEST_FUNCTION(directory_redirect) {
    auto conf = setup();
    fostlib::http::server::request req("GET", "/empty");
    auto [resp, status] = fostlib::urlhandler::static_server(
            conf, "empty", req, fostlib::host{});
    FSL_CHECK_EQ(status, 302);
    FSL_CHECK_EQ(resp->headers()["Location"].value(), "/empty/");
}
