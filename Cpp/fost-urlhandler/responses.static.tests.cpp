/**
    Copyright 2019-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>
#include <fost/test>


namespace {


    /// Set up some proper MIME content types
    fostlib::setting<fostlib::string> const c_mime_html{
            "responses.static.tests.cpp", "MIME", ".html", "text/html"};


    /// Set up the tests. Most of this only needs to be run once,
    /// but it doesn't matter if it runs multiple times
    fostlib::json setup() {
        auto const root = fostlib::coerce<fostlib::fs::path>(
                                  fostlib::test::c_files_folder.value())
                / "static-files";
        fostlib::fs::create_directories(root);
        fostlib::fs::create_directory(root / "empty");
        fostlib::utf::save_file(root / "index.html", "html");
        fostlib::utf::save_file(root / "index.txt", "text");
        fostlib::json conf;
        fostlib::insert(conf, "view", "fost.static");
        fostlib::insert(conf, "configuration", "root", root);
        return conf;
    }


    /// Fetch the response body as text
    fostlib::string content(const fostlib::mime &body) {
        fostlib::string text;
        for (auto part : body) {
            text += std::string{
                    reinterpret_cast<char const *>(part.first),
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
    auto [resp, status] = fostlib::urlhandler::view::execute(
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
            fostlib::urlhandler::view::execute(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK_EQ(content(*resp), "html");
}


/// Serve a custom index file from the directory
FSL_TEST_FUNCTION(custom_default_file) {
    auto conf = setup();
    fostlib::insert(conf, "configuration", "index", "index.txt");
    fostlib::http::server::request req("GET", "/");
    auto [resp, status] =
            fostlib::urlhandler::view::execute(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/plain");
    FSL_CHECK_EQ(content(*resp), "text");
}


/// Fail to serve a default files from the directory
FSL_TEST_FUNCTION(default_file_not_found) {
    auto conf = setup();
    fostlib::http::server::request req("GET", "/empty/");
    auto [resp, status] = fostlib::urlhandler::view::execute(
            conf, "empty/", req, fostlib::host{});
    FSL_CHECK_EQ(status, 403);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
}
FSL_TEST_FUNCTION(custom_default_file_not_found) {
    auto conf = setup();
    fostlib::insert(conf, "configuration", "index", "non-existant.txt");
    fostlib::http::server::request req("GET", "/");
    auto [resp, status] =
            fostlib::urlhandler::view::execute(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 403);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
}


/// Directory redirects without trailing slash
FSL_TEST_FUNCTION(directory_redirect) {
    auto conf = setup();
    fostlib::http::server::request req("GET", "/empty");
    auto [resp, status] = fostlib::urlhandler::view::execute(
            conf, "empty", req, fostlib::host{});
    FSL_CHECK_EQ(status, 302);
    FSL_CHECK_EQ(resp->headers()["Location"].value(), "/empty/");
}


/// Can customise the directory serving view
FSL_TEST_FUNCTION(directory_view_forbidden) {
    auto conf = setup();
    fostlib::insert(
            conf, "configuration", "directory", "view", "fost.response.500");
    fostlib::http::server::request req("GET", "/empty/");
    auto [resp, status] = fostlib::urlhandler::view::execute(
            conf, "empty/", req, fostlib::host{});
    FSL_CHECK_EQ(status, 500);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "text/html");
}

/// Forgetting root directory throws
FSL_TEST_FUNCTION(directory_view_no_root) {
    auto conf = setup();
    fostlib::insert(
            conf, "configuration", "directory", "view",
            "fost.static.directory.json");
    fostlib::http::server::request req("GET", "/empty/");
    FSL_CHECK_EXCEPTION(
            fostlib::urlhandler::view::execute(
                    conf, "empty/", req, fostlib::host{}),
            fostlib::exceptions::not_implemented &);
}


/// Empty directory gives empty JSON when served as JSON
FSL_TEST_FUNCTION(empty_directory_view_json) {
    auto conf = setup();
    fostlib::insert(
            conf, "configuration", "directory", "view",
            "fost.static.directory.json");
    fostlib::insert(
            conf, "configuration", "directory", "configuration", "root",
            conf["configuration"]["root"]);
    fostlib::http::server::request req("GET", "/empty/");
    auto [resp, status] = fostlib::urlhandler::view::execute(
            conf, "empty/", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "application/json");
    FSL_CHECK_EQ(content(*resp), "{}");
}


/// Folders with files give us a listing
FSL_TEST_FUNCTION(directory_view_json) {
    auto conf = setup();
    fostlib::insert(
            conf, "configuration", "directory", "view",
            "fost.static.directory.json");
    fostlib::insert(
            conf, "configuration", "directory", "configuration", "root",
            conf["configuration"]["root"]);
    fostlib::insert(
            conf, "configuration", "directory", "configuration", "index",
            "non-existant.txt");
    fostlib::http::server::request req("GET", "/");
    auto [resp, status] =
            fostlib::urlhandler::view::execute(conf, "", req, fostlib::host{});
    FSL_CHECK_EQ(status, 200);
    FSL_CHECK_EQ(resp->headers()["Content-Type"].value(), "application/json");
    auto const listing = fostlib::json::parse(content(*resp));
    FSL_CHECK_EQ(listing.size(), 3);
    FSL_CHECK(listing.has_key("index.html"));
    FSL_CHECK_EQ(listing["index.html"]["directory"], fostlib::json{false});
    FSL_CHECK_EQ(listing["index.html"]["path"], "index.html");
    FSL_CHECK(listing.has_key("empty"));
    FSL_CHECK_EQ(listing["empty"]["directory"], fostlib::json{true});
    FSL_CHECK_EQ(listing["empty"]["path"], "empty/");
}
