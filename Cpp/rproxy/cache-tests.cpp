#include <proxy/detail/exports.hpp>
#include <proxy/cache.hpp>
#include <fost/test>


namespace {
    const fostlib::setting<fostlib::string> test_cache(
        "cache-tests.cpp", proxy::c_cache_dir,
        fostlib::coerce<fostlib::string>(
            (boost::filesystem::temp_directory_path() /
                boost::filesystem::unique_path()).wstring()));
}


FSL_TEST_SUITE(cache);


