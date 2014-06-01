#include <proxy/stats.hpp>
#include <fost/counter>
#include <fost/threading>


namespace {


    /// The overall number of requests made
    fostlib::counter g_requests;

    /// A shared mutex (multiple readers, one writer)
    boost::shared_mutex g_mutex;

    /// Statistics for a given resource
    struct resource {
        fostlib::url address;
        fostlib::counter origin_requests, origin_failures,
            cache_hits, cache_misses;
    };

    /// The store
    typedef std::map<fostlib::string, std::unique_ptr<resource>> stats_type;


}


void proxy::stats::request(
    const fostlib::string &hash, const fostlib::url &resource
) {
    ++g_requests;
}

