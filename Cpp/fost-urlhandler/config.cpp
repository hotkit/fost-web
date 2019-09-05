/**
    Copyright 2008-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const fostlib::setting<fostlib::json> fostlib::urlhandler::c_hosts(
        "urlhandler/routing.cpp",
        "webserver",
        "hosts",
        fostlib::json::object_t(),
        true);
const fostlib::setting<fostlib::json> fostlib::urlhandler::c_views(
        "urlhandler/routing.cpp",
        "webserver",
        "views",
        fostlib::json::object_t(),
        true);
