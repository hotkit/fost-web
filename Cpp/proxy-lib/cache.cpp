#include <proxy/cache.hpp>
#include <fost/log>


const fostlib::setting<fostlib::string> proxy::c_cache_dir(
    "cache.cpp", "proxy", "Cache directory",
    fostlib::coerce<fostlib::string>(
        (boost::filesystem::temp_directory_path() /
            boost::filesystem::unique_path()).wstring()),
    true);


boost::filesystem::wpath proxy::root() {
    boost::filesystem::wpath root(
        fostlib::coerce<boost::filesystem::wpath>(c_cache_dir.value()));
    if ( !boost::filesystem::exists(root) ) {
        boost::filesystem::create_directory(root);
        fostlib::log::warning("Created directory", root);
    }
    return root;
}


void proxy::flush_cache() {
    boost::filesystem::wpath path(root());
    fostlib::log::info()
        ("root", path)
        ("removed", boost::filesystem::remove_all(path));
}

