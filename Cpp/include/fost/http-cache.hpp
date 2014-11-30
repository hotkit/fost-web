/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_HTTP_CACHE_HPP
#define FOST_HTTP_CACHE_HPP
#pragma once


#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/file>
#include <fost/jsondb>


namespace fostlib {


    /// Location for the cache data
    FOST_RPROXY_DECLSPEC
    extern const setting<string> c_cache_dir;


    /// Return the root path for the cache
    FOST_RPROXY_DECLSPEC
    boost::filesystem::wpath root();


    /// Return the cache address hash for the response
    FOST_RPROXY_DECLSPEC
    string hash(const http::user_agent::request &);
    /// Return the cache address hash for the response
    FOST_RPROXY_DECLSPEC
    string hash(const http::user_agent::response &);

    /// Return the variant hash for the set of headers
    FOST_RPROXY_DECLSPEC
    string variant(const mime::mime_headers &);
    /// Return the variant hash for the headers given a Vary header
    FOST_RPROXY_DECLSPEC
    string variant(const mime::mime_headers &,
        const string &vary);


    /// Flush the entire cache
    FOST_RPROXY_DECLSPEC
    void flush_cache();


    /// Returns the cache database (if we already know the root location)
    FOST_RPROXY_DECLSPEC
    boost::shared_ptr<jsondb> cache_db(
        const boost::filesystem::wpath &,
        const nullable<string> &subdb = null);


    /// Returns the cache database
    inline boost::shared_ptr<jsondb> cache_db() {
        return cache_db(root());
    }
    /// Returns the file sub-database
    inline boost::shared_ptr<jsondb> cache_db(
        const string &sdb
    ) {
        return cache_db(root(), sdb);
    }


    /// Returns the cache DB entry for the hash
    FOST_RPROXY_DECLSPEC
    json db_entry(const string &hash);


    /// Save a response into the cache -- returns path to save body at
    FOST_RPROXY_DECLSPEC
    boost::filesystem::wpath save_entry(
        const http::server::request &,
        const http::user_agent::response &);
    /// Update an entry -- cache hit
    FOST_RPROXY_DECLSPEC
    boost::filesystem::wpath update_entry(
        const string &h, const string &vh);


}


#endif // FOST_HTTP_CACHE_HPP
