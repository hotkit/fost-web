/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/rproxy>
#include <fost/test>


namespace {
    boost::shared_ptr<fostlib::binary_body> empty_body() {
        return boost::shared_ptr<fostlib::binary_body>(new fostlib::binary_body);
    }
}


FSL_TEST_SUITE(cache_db);


FSL_TEST_FUNCTION(empty_db) {
    fostlib::flush_cache();
    boost::shared_ptr<fostlib::jsondb> db(fostlib::cache_db());
    fostlib::jsondb::local trans(*db);
    FSL_CHECK_EQ(trans["file-db"],
        fostlib::json(fostlib::json::object_t()));
    FSL_CHECK_EQ(db, fostlib::cache_db());
}


FSL_TEST_FUNCTION(save_response) {
    fostlib::flush_cache();
    fostlib::http::server::request req("GET", "/");
    fostlib::http::user_agent::response r("GET",
        fostlib::url("http://example.com/"), 200, empty_body());
    FSL_CHECK_EQ(
        fostlib::save_entry(req, r),
        boost::filesystem::wpath("d3") /
            L"c8eae015367cfdcd581ddbef8fa58f"
            L"-d41d8cd98f00b204e9800998ecf8427e");
    boost::shared_ptr<fostlib::jsondb> db(fostlib::cache_db());
    fostlib::jsondb::local trans(*db);
    FSL_CHECK_EQ(trans["file-db"]["d3"]["db"],
        fostlib::coerce<fostlib::json>(
            boost::filesystem::wpath("d3") / L"file-db.json"));
    FSL_CHECK_EQ(
        fostlib::save_entry(req, r),
        boost::filesystem::wpath("d3") /
            L"c8eae015367cfdcd581ddbef8fa58f"
            L"-d41d8cd98f00b204e9800998ecf8427e");
}


FSL_TEST_FUNCTION(update_response) {
    fostlib::flush_cache();
    FSL_CHECK_NOTHROW(fostlib::save_entry(
        fostlib::http::server::request("GET", "/"),
        fostlib::http::user_agent::response("GET",
            fostlib::url("http://example.com/"), 200, empty_body())));
    const boost::filesystem::wpath file(fostlib::update_entry(
        "d3c8eae015367cfdcd581ddbef8fa58f",
        "d41d8cd98f00b204e9800998ecf8427e"));
    boost::shared_ptr<fostlib::jsondb> fdb(
        fostlib::cache_db(fostlib::string("d3")));
}


FSL_TEST_FUNCTION(retrieve_response) {
    fostlib::flush_cache();
    FSL_CHECK_NOTHROW(fostlib::save_entry(
        fostlib::http::server::request("GET", "/"),
        fostlib::http::user_agent::response("GET",
            fostlib::url("http://example.com/"), 200, empty_body())));
    fostlib::json entry(fostlib::db_entry(
        "d3c8eae015367cfdcd581ddbef8fa58f"));
    FSL_CHECK_NEQ(entry, fostlib::json());
    FSL_CHECK_EQ(entry["hash"],
        fostlib::json("d3c8eae015367cfdcd581ddbef8fa58f"));
}


FSL_TEST_FUNCTION(cache_miss) {
    fostlib::flush_cache();
    fostlib::save_entry(
        fostlib::http::server::request("GET", "/"),
        fostlib::http::user_agent::response("GET",
            fostlib::url("http://example.com/"), 200, empty_body()));
    fostlib::json entry(fostlib::db_entry(
        "00e39d220ff38421b6dd61a998975b28"));
    FSL_CHECK_EQ(entry, fostlib::json());
}

