#include <proxy/cache.hpp>
#include <fost/datetime>
#include <fost/insert>
#include <fost/log>


namespace {
    typedef std::pair<fostlib::string, fostlib::string> dbkeys;
     dbkeys dblookup(const fostlib::string &hash) {
        const fostlib::string fdb_name(hash.substr(0, 2));
        const fostlib::string fdb_key(hash.substr(2));
        return std::make_pair(fdb_name, fdb_key);
    }
}


fostlib::json proxy::db_entry(const fostlib::hex_string &hash) {
    fostlib::string h(fostlib::coerce<fostlib::string>(hash));
    const dbkeys keys(dblookup(h));
    std::shared_ptr<fostlib::jsondb> db(cache_db(keys.first));
    if ( db ) {
        fostlib::jsondb::local trans(*db);
        return trans["file"][keys.second];
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
    const dbkeys keys(dblookup(h));

    fostlib::json description;
    const boost::filesystem::wpath pathname(
        fostlib::coerce<boost::filesystem::wpath>(keys.first) /
        fostlib::coerce<boost::filesystem::wpath>(keys.second + "-" + vh));
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
    std::shared_ptr<fostlib::jsondb> db(cache_db(keys.first));
    fostlib::jsondb::local trans(*db);
    trans.set(fostlib::jcursor("file") / keys.second, description);
    trans.commit();
    return pathname;
}


boost::filesystem::wpath proxy::update_entry(
    const fostlib::http::user_agent::request &request
) {
    const fostlib::string h(fostlib::coerce<fostlib::string>(hash(request)));
    const fostlib::string vh("d41d8cd98f00b204e9800998ecf8427e");
    const dbkeys keys(dblookup(h));

    fostlib::log::debug()
        ("hash", h)
        ("variant", vh)
        ("dbkeys", "first", keys.first)
        ("dbkeys", "second", keys.second);

    std::shared_ptr<fostlib::jsondb> db(cache_db(keys.first));
    fostlib::jsondb::local trans(*db);
    fostlib::jcursor dbloc("file");
    dbloc /= keys.second;
    trans.set(dbloc / "variant" / vh / "accessed", fostlib::timestamp::now());
    trans.commit();
    return fostlib::coerce<boost::filesystem::wpath>(
        trans[dbloc / "variant"/ vh / "pathname"]);
}

