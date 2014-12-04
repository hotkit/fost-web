/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/rproxy>
#include <fost/test>


namespace {
    const fostlib::setting<fostlib::string> test_cache(
        "cache-tests.cpp", fostlib::c_cache_dir,
        fostlib::coerce<fostlib::string>(fostlib::unique_filename()));
}


FSL_TEST_SUITE(cache);


