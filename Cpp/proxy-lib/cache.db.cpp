#include <proxy/cache.hpp>


namespace {
}


fostlib::jsondb &proxy::cache_db(const boost::filesystem::wpath &root) {
    static fostlib::jsondb db;
    return db;
}


fostlib::json proxy::db_entry(const fostlib::hex_string &hash) {
    return fostlib::json();
}

