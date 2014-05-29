#include <proxy/cache.hpp>
#include <fost/datetime>
#include <fost/insert>


fostlib::json proxy::db_entry(const fostlib::hex_string &hash) {
    fostlib::string h(fostlib::coerce<fostlib::string>(hash));
    fostlib::string fdb_name(h.substr(0, 2));
    fostlib::string fdb_key(h.substr(2));
    std::shared_ptr<fostlib::jsondb> db(cache_db(fdb_name));
    if ( db ) {
        fostlib::jsondb::local trans(*db);
        return trans["file"][fdb_key];
    }
    return fostlib::json();
}


boost::filesystem::wpath proxy::save_entry(
    const fostlib::http::server::request &request,
    const fostlib::http::user_agent::response &response
) {
    const fostlib::string h(fostlib::coerce<fostlib::string>(hash(response)));
    const fostlib::string vh("d41d8cd98f00b204e9800998ecf8427e");
    // fostlib::string vh(fostlib::coerce<fostlib::string>(
    //     proxy::variant(response.headers())));

    const fostlib::string fdb_name(h.substr(0, 2));
    const fostlib::string fdb_key(h.substr(2));
    fostlib::json description;
    const boost::filesystem::wpath pathname(
        fostlib::coerce<boost::filesystem::wpath>(fdb_name) /
        fostlib::coerce<boost::filesystem::wpath>(fdb_key + "-" + vh));
    fostlib::insert(description, "address", response.address());
    fostlib::insert(description, "hash", h);
    fostlib::json variant;
    fostlib::insert(variant, "created", fostlib::timestamp::now());
    fostlib::insert(variant, "accessed", variant["created"]);
    fostlib::insert(variant, "pathname", pathname);
    fostlib::insert(variant, "request", "headers", request.data()->headers());
    fostlib::insert(variant, "response", "status", response.status());
    fostlib::insert(variant, "response", "headers", response.headers());
    fostlib::insert(description, "variant", vh, variant);
    std::shared_ptr<fostlib::jsondb> db(cache_db(fdb_name));
    fostlib::jsondb::local trans(*db);
    trans.set(fostlib::jcursor("file") / fdb_key, description);
    trans.commit();
    return pathname;
}

