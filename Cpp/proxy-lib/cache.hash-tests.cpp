#include <proxy/detail/exports.hpp>
#include <proxy/cache.hpp>
#include <fost/test>


FSL_TEST_SUITE(cache_db);


FSL_TEST_FUNCTION(empty_db) {
    fostlib::jsondb &db(proxy::cache_db());
    fostlib::jsondb::local trans(db);
}

