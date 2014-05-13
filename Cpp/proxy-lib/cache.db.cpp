#include <proxy/cache.hpp>
#include <fost/insert>


namespace {
    unsigned char undigit(char d) {
        if ( d >= '0' && d <= '9' )
            return d - '0';
        else if ( d >= 'A' && d <= 'F' )
            return d - 'A' + 0xa;
        else if ( d >= 'a' && d <= 'f' )
            return d - 'a' + 0xa;
        throw fostlib::exceptions::out_of_range< int >(
            "Hex digit is not valid", '0', 'F', d);
    }
    unsigned char unhex(char c1, char c2) {
        return (undigit(c1) << 4) + undigit(c2);
    }
    boost::mutex g_mutex;
}


std::shared_ptr<fostlib::jsondb> proxy::cache_db(
    const boost::filesystem::wpath &root,
    const fostlib::nullable<fostlib::string> &subdb
) {
    boost::mutex::scoped_lock lock(g_mutex);
    static std::shared_ptr<fostlib::jsondb> dbp;
    static std::vector<std::shared_ptr<fostlib::jsondb>> sdbs;

    boost::filesystem::wpath cache_file(root / "cache.json");
    if ( !dbp || dbp->filename().value() != cache_file ) {
        fostlib::json cache;
        fostlib::insert(cache, "file-db", fostlib::json::object_t());
        dbp = std::make_shared<fostlib::jsondb>(cache_file, cache);
        sdbs.clear();
        sdbs.resize(256);
    }
    if ( subdb.isnull() ) {
        return dbp;
    } else {
        fostlib::jsondb::local trans(*dbp);
        std::size_t slot(unhex(subdb.value()[0], subdb.value()[1]));
        if ( trans["file-db"][subdb.value()].isnull() ) {
            boost::filesystem::wpath fdb_path(root/
                fostlib::coerce<boost::filesystem::wpath>(subdb.value()));
            boost::filesystem::wpath fdb_pathname(fdb_path / "file-db.json");
            boost::filesystem::create_directory(fdb_path);
            fostlib::json content;
            fostlib::insert(content, "file", fostlib::json::object_t());
            sdbs[slot] = std::make_shared<fostlib::jsondb>(
                fdb_pathname, content);
            trans.insert(fostlib::jcursor("file-db") / subdb.value() / "db",
                fostlib::coerce<fostlib::json>(fdb_pathname));
            trans.commit();
        } else if ( !sdbs[slot] ) {
            sdbs[slot] = std::make_shared<fostlib::jsondb>(
                fostlib::coerce<boost::filesystem::wpath>(
                    trans["file-db"][subdb.value()]["db"]));
        }
        return sdbs[slot];
    }
}


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

