/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>
#include <fost/threading>


namespace {
    typedef fostlib::threadsafe_store<urlhandler::view*> view_store;
    view_store &views() {
        static view_store store;
        return store;
    }
}

urlhandler::view::view(const fostlib::string &name) {
    views().add(name, this);
}

urlhandler::view::~view() {
    // Allow the view to leak as they'll only be removed on termination
}


const urlhandler::view &urlhandler::view::view_for(const fostlib::string &name) {
    view_store::found_t found(views().find(name));
    if ( found.size() == 1 )
        return *found[0];
    else
        throw fostlib::exceptions::not_implemented(
            "urlhandler::view::view_for(const fostlib::string &)",
            "Where zero or more than 1 views are found");
}

