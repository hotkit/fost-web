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
}


fostlib::jsondb &proxy::cache_db(const boost::filesystem::wpath &root,
        fostlib::nullable<fostlib::string> subdb) {
    static std::unique_ptr<fostlib::jsondb> dbp;
    static std::vector<std::unique_ptr<fostlib::jsondb>> sdbs;

    if ( !dbp || dbp->filename().value() != root / "cache.json" ) {
        fostlib::json cache;
        fostlib::insert(cache, "file-db", fostlib::json::object_t());
        dbp.reset(new fostlib::jsondb(root / "cache.json", cache));
        sdbs.clear();
        sdbs.reserve(256);
    }
    if ( subdb.isnull() ) {
        return *dbp;
    } else {
        fostlib::jsondb::local trans(*dbp);
        std::size_t slot(unhex(subdb.value()[0], subdb.value()[1]));
        if ( trans["file-db"][subdb.value()].isnull() ) {
            boost::filesystem::wpath fdb_path(root/
                fostlib::coerce<boost::filesystem::wpath>(subdb.value()));
            boost::filesystem::create_directory(fdb_path);
            sdbs[slot].reset(
                new fostlib::jsondb(fdb_path / "file-db.json", fostlib::json()));
            trans.insert(fostlib::jcursor("file-db") / subdb.value() / "db",
                fostlib::coerce<fostlib::json>(fdb_path / "file-db.json"));
            trans.commit();
        } else {
            throw fostlib::exceptions::not_implemented("sub databases");
        }
        return *sdbs[slot];
    }
}


fostlib::json proxy::db_entry(const fostlib::hex_string &hash) {
    return fostlib::json();
}


boost::filesystem::wpath proxy::save_entry(
        const fostlib::http::user_agent::response &r) {
    fostlib::string h(fostlib::coerce<fostlib::string>(hash(r)));
    fostlib::string fdb_name(h.substr(0, 2));
    fostlib::jsondb &db(cache_db(fdb_name));
    fostlib::jsondb::local trans(db);
    return root() /
        fostlib::coerce<boost::filesystem::wpath>(fdb_name) /
        fostlib::coerce<boost::filesystem::wpath>(h.substr(2));
}

