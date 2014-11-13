/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/cache.hpp>
#include <fost/datetime>
#include <fost/insert>
#include <fost/log>


namespace {
    typedef std::pair<fostlib::string, fostlib::string> dbkeys;
     dbkeys dblookup(const fostlib::string &hash) {
        const fostlib::string fdb_name(hash.substr(0, 2));
        const fostlib::string fdb_key(hash.substr(2));
        return std::make_pair(fdb_name, fdb_key);
    }
}


fostlib::json fostlib::db_entry(const fostlib::string &hash) {
    const dbkeys keys(dblookup(hash));
    std::shared_ptr<fostlib::jsondb> db(cache_db(keys.first));
    if ( db ) {
        fostlib::jsondb::local trans(*db);
        return trans["file"][keys.second];
    }
    return fostlib::json();
}


boost::filesystem::wpath fostlib::save_entry(
    const fostlib::http::server::request &request,
    const fostlib::http::user_agent::response &response
) {
    const fostlib::string h(hash(response));
    const fostlib::string vh("d41d8cd98f00b204e9800998ecf8427e");
    // fostlib::string vh(fostlib::coerce<fostlib::string>(
    //     fostlib::variant(response.headers())));
    const dbkeys keys(dblookup(h));

    const boost::filesystem::wpath pathname(
        fostlib::coerce<boost::filesystem::wpath>(keys.first) /
        fostlib::coerce<boost::filesystem::wpath>(keys.second + "-" + vh));
    fostlib::json variant;
    fostlib::insert(variant, "accessed", fostlib::timestamp::now());
    fostlib::insert(variant, "updated", variant["accessed"]);
    fostlib::insert(variant, "pathname", pathname);
    fostlib::insert(variant, "response", "status", response.status());
    fostlib::insert(variant, "response", "headers", response.headers());
    std::shared_ptr<fostlib::jsondb> db(cache_db(keys.first));
    fostlib::jsondb::local trans(*db);
    fostlib::jcursor base_loc("file", keys.second);
    if ( trans.has_key(base_loc) ) {
        fostlib::jcursor variant_loc(base_loc, "variant", vh);
        if ( trans.has_key(variant_loc) ) {
            fostlib::insert(variant, "created", trans[variant_loc]["created"]);
            trans.set(variant_loc, variant);
        } else {
            fostlib::insert(variant, "created", variant["accessed"]);
            trans.set(variant_loc, variant);
        }
    } else {
        fostlib::json description;
        fostlib::insert(variant, "created", variant["accessed"]);
        fostlib::insert(description, "address", response.address());
        fostlib::insert(description, "hash", h);
        fostlib::insert(description, "variant", vh, variant);
        trans.set(base_loc, description);
    }
    trans.commit();
    return pathname;
}


boost::filesystem::wpath fostlib::update_entry(
    const fostlib::string &h, const fostlib::string &vh
) {
    const dbkeys keys(dblookup(h));

    std::shared_ptr<fostlib::jsondb> db(cache_db(keys.first));
    fostlib::jsondb::local trans(*db);
    fostlib::jcursor dbloc(fostlib::jcursor("file") / keys.second / "variant" / vh);
    trans.set(dbloc / "accessed", fostlib::timestamp::now());
    trans.commit();
    return fostlib::coerce<boost::filesystem::wpath>(
        trans[dbloc / "pathname"]);
}

