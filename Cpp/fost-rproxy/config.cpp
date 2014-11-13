/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <proxy/config.hpp>


const fostlib::setting<fostlib::string> proxy::c_cache_dir(
    "cache.cpp", "proxy", "Cache directory",
    "/tmp/proxy-lib/", true);
