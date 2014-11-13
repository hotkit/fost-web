#include <fost/core>


namespace fostlib {


    /// Location for the cache data
    extern const fostlib::setting<fostlib::string> c_cache_dir;
    /// The S3 account name to use
    extern const fostlib::setting<fostlib::string> c_account;
    /// The bucket to fetch from and upload to
    extern const fostlib::setting<fostlib::string> c_bucket;


}

