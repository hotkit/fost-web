#include <proxy/cache.hpp>
#include <fost/insert>


namespace {
}


fostlib::jsondb &proxy::cache_db(const boost::filesystem::wpath &root) {
    static std::unique_ptr<fostlib::jsondb> dbp;
    if ( !dbp || dbp->filename().value() != root ) {
        fostlib::json cache;
        fostlib::insert(cache, "sub-caches", fostlib::json::object_t());
        dbp.reset(new fostlib::jsondb(root / "cache.json", cache));
    }
    return *dbp;
}


fostlib::json proxy::db_entry(const fostlib::hex_string &hash) {
    return fostlib::json();
}

