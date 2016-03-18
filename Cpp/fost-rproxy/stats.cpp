/*
    Copyright 2014-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-rproxy.hpp"
#include <fost/stats.hpp>


using namespace fostlib;


namespace {
    boost::mutex g_mutex;
    fostlib::json g_statistics = json::object_t();

    struct statistics {
        statistics(const json &) {
        }

        bool operator () (const log::message &m) {
            if ( m.level() == fostlib::log::g_stats_level ) {
                boost::mutex::scoped_lock lock(g_mutex);
                string key(coerce<string>(m.body()["key"]));
                for ( json::const_iterator it(m.body()["data"].begin());
                        it != m.body()["data"].end(); ++it ) {
                    string itk(coerce<string>(it.key()));
                    if ( g_statistics[key].has_key(itk) ) {
                        (jcursor(key) / itk).replace(g_statistics, *it);
                    } else {
                        insert(g_statistics, key, itk, *it);
                    }
                }
                for ( json::const_iterator it(m.body()["add"].begin());
                        it != m.body()["add"].end(); ++it ) {
                    string itk(coerce<string>(it.key()));
                    if ( g_statistics[key].has_key(itk) ) {
                        (jcursor(key) / itk).replace(g_statistics,
                            coerce<int64_t>(g_statistics[key][itk]) +
                            coerce<int64_t>(*it));
                    } else {
                        insert(g_statistics, key, itk, *it);
                    }
                }
                return false;
            } else {
                return true;
            }
        }
    };

    const log::global_sink<statistics> std_out("statistics");


//     json fetch_stats(const json &) {
//         boost::mutex::scoped_lock lock(g_mutex);
//         return g_statistics;
//     }


}


void fostlib::stats::load(const json &old) {
    boost::mutex::scoped_lock lock(g_mutex);
    g_statistics = old;
}

