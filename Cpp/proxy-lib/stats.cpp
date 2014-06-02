#include <proxy/stats.hpp>
#include <varanus/obor.hpp>


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
                return true;
            } else {
                return true;
            }
        }
    };

    const log::global_sink<statistics> std_out("statistics");


    json fetch_stats(const json &) {
        boost::mutex::scoped_lock lock(g_mutex);
        return g_statistics;
    }

    const varanus::obor c_statistics("proxy.statistics", fetch_stats);


}


void proxy::stats::load(const json &old) {
    boost::mutex::scoped_lock lock(g_mutex);
    g_statistics = old;
}

