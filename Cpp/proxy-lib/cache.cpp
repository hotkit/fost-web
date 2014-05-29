#include <proxy/cache.hpp>
#include <fost/log>


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
    std::shared_ptr<fostlib::jsondb> g_dbp;
    std::vector<std::shared_ptr<fostlib::jsondb>> g_sdbs;

    void reset(boost::mutex::scoped_lock &,
        const boost::filesystem::wpath &cache_file) {
        fostlib::json cache;
        fostlib::insert(cache, "file-db", fostlib::json::object_t());
        g_dbp = std::make_shared<fostlib::jsondb>(cache_file, cache);
        g_sdbs.clear();
        g_sdbs.resize(256);
    }
}


const fostlib::setting<fostlib::string> proxy::c_cache_dir(
    "cache.cpp", "proxy", "Cache directory",
    "/tmp/proxy-lib/", true);


boost::filesystem::wpath proxy::root() {
    fostlib::utf8_string root(fostlib::coerce<fostlib::utf8_string>(
        c_cache_dir.value()));
    return root.underlying();
}


void proxy::flush_cache() {
    boost::mutex::scoped_lock lock(g_mutex);
    boost::filesystem::wpath path(root());
    fostlib::log::info()
        ("root", path)
        ("removed", boost::filesystem::remove_all(path));
    boost::filesystem::create_directory(path);
    reset(lock, path / "cache.json");
}


std::shared_ptr<fostlib::jsondb> proxy::cache_db(
    const boost::filesystem::wpath &root,
    const fostlib::nullable<fostlib::string> &subdb
) {
    boost::mutex::scoped_lock lock(g_mutex);

    boost::filesystem::wpath cache_file(root / "cache.json");
    if ( !g_dbp || g_dbp->filename().value() != cache_file ) {
        fostlib::log::warning()
            ("", "Need to load root cache database in RAM")
            ("create-directory", "path", root)
            ("create-directory", "result", boost::filesystem::create_directory(root))
            ("cache-file", cache_file);
        reset(lock, cache_file);
    }
    if ( subdb.isnull() ) {
        fostlib::log::debug("Returning the root database");
        return g_dbp;
    } else {
        fostlib::jsondb::local trans(*g_dbp);
        std::size_t slot(unhex(subdb.value()[0], subdb.value()[1]));
        if ( trans["file-db"][subdb.value()].isnull() ) {
            boost::filesystem::wpath fdb_path(
                fostlib::coerce<boost::filesystem::wpath>(subdb.value()));
            boost::filesystem::wpath fdb_pathname(fdb_path / "file-db.json");
            fostlib::log::debug()
                ("", "Creating sub database")
                ("sub-db", subdb.value())
                ("fdb_pathname", fdb_pathname)
                ("create-folder",
                    boost::filesystem::create_directory(root / fdb_path));
            fostlib::json content;
            fostlib::insert(content, "file", fostlib::json::object_t());
            g_sdbs[slot] = std::make_shared<fostlib::jsondb>(
                root / fdb_pathname, content);
            trans.insert(fostlib::jcursor("file-db") / subdb.value() / "db",
                fostlib::coerce<fostlib::json>(fdb_pathname));
            trans.commit();
            fostlib::log::debug("New database committed");
        } else if ( !g_sdbs[slot] ) {
            fostlib::log::debug()
                ("", "Loading database")
                ("sub-db", subdb.value())
                ("pathname", trans["file-db"][subdb.value()]["db"]);
            g_sdbs[slot] = std::make_shared<fostlib::jsondb>(
                root / fostlib::coerce<boost::filesystem::wpath>(
                    trans["file-db"][subdb.value()]["db"]));
        }
        return g_sdbs[slot];
    }
}

