#include <proxy/cache.hpp>
#include <fost/insert>


fostlib::json proxy::db_entry(const fostlib::hex_string &hash) {
    return fostlib::json();
}


boost::filesystem::wpath proxy::save_entry(
        const fostlib::http::user_agent::response &response) {
    fostlib::string h(fostlib::coerce<fostlib::string>(hash(response)));
    fostlib::string vh(fostlib::coerce<fostlib::string>(
        proxy::variant(response.headers())));

    fostlib::string fdb_name(h.substr(0, 2));
    fostlib::string fdb_key(h.substr(2));
    fostlib::json description;
    boost::filesystem::wpath pathname(root() /
        fostlib::coerce<boost::filesystem::wpath>(fdb_name) /
        fostlib::coerce<boost::filesystem::wpath>(fdb_key + "-" + vh));
    fostlib::insert(description, "address", response.address());
    fostlib::insert(description, "hash", h);
    fostlib::json variant;
    fostlib::insert(variant, "pathname", pathname);
    fostlib::insert(variant, "response", "status", response.status());
    fostlib::insert(variant, "response", "headers", response.headers());
    fostlib::insert(description, "variant", vh, variant);
    std::shared_ptr<fostlib::jsondb> db(cache_db(fdb_name));
    fostlib::jsondb::local trans(*db);
    trans.set(fostlib::jcursor("file") / fdb_key, description);
    trans.commit();
    return pathname;
}

