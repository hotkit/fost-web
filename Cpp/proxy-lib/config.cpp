#include <proxy/config.hpp>
#include <fost/aws>


const fostlib::setting<fostlib::string> proxy::c_cache_dir(
    "cache.cpp", "proxy", "Cache directory",
    "/tmp/proxy-lib/", true);


const fostlib::setting<fostlib::string> proxy::c_account(
    "proxy-lib/config.cpp",
    fostlib::aws::s3::bucket::s_account_name, "wos");


const fostlib::setting<fostlib::string> proxy::c_bucket(
    "proxy-lib/config.cpp",
    "Uploader", "bucket", "fbwc2014", true);

