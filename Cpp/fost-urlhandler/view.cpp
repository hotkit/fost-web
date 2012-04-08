/*
    Copyright 2011-2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/threading>


namespace {
    typedef fostlib::threadsafe_store<fostlib::urlhandler::view*> view_store;
    view_store &views() {
        static view_store store;
        return store;
    }
}

fostlib::urlhandler::view::view(const fostlib::string &name) {
    views().add(name, this);
}

fostlib::urlhandler::view::~view() {
    // Allow the view to leak as they'll only be removed on termination
}


std::pair<fostlib::string, fostlib::json> fostlib::urlhandler::view::find_view(
    const fostlib::string &view_name, const fostlib::json &view_config
) {
    fostlib::json views(c_views.value());
    std::pair<fostlib::string, fostlib::json> final(view_name, view_config);
    while ( views.has_key(final.first) )
        final = std::make_pair(
            fostlib::coerce<fostlib::string>(views[final.first]["view"]),
            views[final.first]["configuration"]);
    return final;
}


const fostlib::urlhandler::view &fostlib::urlhandler::view::view_for(
        const fostlib::string &name
) {
    view_store::found_t found(views().find(name));
    if ( found.size() == 1 )
        return *found[0];
    else
        throw fostlib::exceptions::not_implemented(
            "urlhandler::view::view_for(const fostlib::string &)",
            "Where zero or more than 1 views are found",
            name);
}

