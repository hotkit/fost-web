#include <proxy/detail/exports.hpp>
#include <proxy/cache.hpp>
#include <fost/test>


FSL_TEST_SUITE(cache_db);


FSL_TEST_FUNCTION(empty_db) {
    proxy::flush_cache();
    std::shared_ptr<fostlib::jsondb> db(proxy::cache_db());
    fostlib::jsondb::local trans(*db);
    FSL_CHECK_EQ(trans["file-db"],
        fostlib::json(fostlib::json::object_t()));
    FSL_CHECK_EQ(db, proxy::cache_db());
}


FSL_TEST_FUNCTION(save_response) {
    proxy::flush_cache();
    fostlib::http::user_agent::response r("GET",
        fostlib::url("http://example.com/"), 200,
        boost::make_shared<fostlib::binary_body>());
    FSL_CHECK_EQ(
        proxy::save_entry(r),
        proxy::root() / "e0" / "e39d220ff38421b6dd61a998975b28"
            "-d41d8cd98f00b204e9800998ecf8427e");
    std::shared_ptr<fostlib::jsondb> db(proxy::cache_db());
    fostlib::jsondb::local trans(*db);
    FSL_CHECK_EQ(trans["file-db"]["e0"]["db"],
        fostlib::coerce<fostlib::json>(proxy::root() / "e0" / "file-db.json"));
    FSL_CHECK_EQ(
        proxy::save_entry(r),
        proxy::root() / "e0" / "e39d220ff38421b6dd61a998975b28"
            "-d41d8cd98f00b204e9800998ecf8427e");
}

