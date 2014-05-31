#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/file>
#include <fost/jsondb>


namespace proxy {


    /// Location for the cache data
    extern const fostlib::setting<fostlib::string> c_cache_dir;


    /// Return the root path for the cache
    boost::filesystem::wpath root();


    /// Return the cache address hash for the response
    fostlib::hex_string hash(const fostlib::http::user_agent::request &);
    /// Return the cache address hash for the response
    fostlib::hex_string hash(const fostlib::http::user_agent::response &);

    /// Return the variant hash for the set of headers
    fostlib::hex_string variant(const fostlib::mime::mime_headers &);
    /// Return the variant hash for the headers given a Vary header
    fostlib::hex_string variant(const fostlib::mime::mime_headers &,
        const fostlib::string &vary);


    /// Flush the entire cache
    void flush_cache();


    /// Returns the cache database (if we already know the root location)
    std::shared_ptr<fostlib::jsondb> cache_db(
        const boost::filesystem::wpath &,
        const fostlib::nullable<fostlib::string> &subdb = fostlib::null);


    /// Returns the cache database
    inline std::shared_ptr<fostlib::jsondb> cache_db() {
        return cache_db(root());
    }
    /// Returns the file sub-database
    inline std::shared_ptr<fostlib::jsondb> cache_db(
        const fostlib::string &sdb
    ) {
        return cache_db(root(), sdb);
    }


    /// Returns the cache DB entry for the hash
    fostlib::json db_entry(const fostlib::hex_string &hash);


    /// Save a response into the cache -- returns path to save body at
    boost::filesystem::wpath save_entry(
        const fostlib::http::server::request &,
        const fostlib::http::user_agent::response &);


}

