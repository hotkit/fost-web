/*
    Copyright 2011-2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/insert>
#include <fost/threading>
#include <fost/urlhandler.hpp>
#include <fost/exception/unexpected_nil.hpp>


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


namespace {
    bool next_view(
        const fostlib::json &views, std::pair<fostlib::string, fostlib::json> &current
    ) {
        if ( views.has_key(current.first) ) {
            current = std::make_pair(
                fostlib::coerce<fostlib::string>(views[current.first]["view"]),
                views[current.first]["configuration"]);
            return true;
        } else {
            fostlib::nullable<fostlib::json> view_setting(
                fostlib::setting<fostlib::json>::value(
                    fostlib::urlhandler::c_views.section(),
                    fostlib::urlhandler::c_views.name() + "/" + current.first,
                    fostlib::null));
            if ( !view_setting.isnull() ) {
                current = std::make_pair(
                    fostlib::coerce<fostlib::string>(view_setting.value()["view"]),
                    view_setting.value()["configuration"]);
                return true;
            } else {
                return false;
            }
        }
    }
}
std::pair<fostlib::string, fostlib::json> fostlib::urlhandler::view::find_view(
    const fostlib::string &view_name, const fostlib::json &view_config
) {
    fostlib::json views(c_views.value());
    std::pair<fostlib::string, fostlib::json> final(view_name, view_config);
    while ( next_view(views, final) ) {
        ;
    }
    return final;
}


const fostlib::urlhandler::view &fostlib::urlhandler::view::view_for(
    const fostlib::string &name
) {
    view_store::found_t found(views().find(name));
    if ( found.size() == 1 ) {
        return *found[0];
    } else {
        fostlib::exceptions::unexpected_nil exception(
            "Where zero or more than 1 views are found");
        insert(exception.data(), "view-name", name);
        insert(exception.data(), "found", found.size());
        throw exception;
    }
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fostlib::urlhandler::view::execute(
    const fostlib::json &configuration,
    const fostlib::string &path,
    fostlib::http::server::request &request,
    const fostlib::host &host
) {
    if ( configuration.isobject() ) {
        auto view_name = coerce<string>(configuration["view"]);
        auto view_fn = find_view(view_name, configuration["configuration"]);
        return view_for(view_fn.first)(view_fn.second, path, request, host);
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "Only implemented for JSON object configurations");
    }
}

