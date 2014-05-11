#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/file>


namespace proxy {


    /// Location for the cache data
    extern const fostlib::setting<fostlib::string> c_cache_dir;


    /// Return the root path for the cache
    boost::filesystem::wpath root();


    /// Return the cache address hash for the request
    fostlib::hex_string hash(const fostlib::http::server::request &req);


    /// Returns the cache entry for the hash
    fostlib::json db_entry(const fostlib::hex_string &hash);


}

