#include <proxy/stats.hpp>
#include <fost/datetime>
#include <fost/threading>


namespace {


    /// A shared mutex (multiple readers, one writer)
    boost::shared_mutex g_mutex;
    /// The number of requests so far
    int64_t g_requests = 0;

    /// Statistics for a given resource
    struct resource {
        boost::mutex mutex;
        fostlib::url address;
        fostlib::timestamp accessed;
        int64_t requests,
            origin_requests, origin_failures,
            cache_hits, cache_misses;
        resource(const fostlib::url &a)
        : address(a), accessed(fostlib::timestamp::now()),
                requests(1), origin_requests(0), origin_failures(0),
                cache_hits(0), cache_misses(0) {
        }
    };

    /// The store
    typedef std::map<fostlib::string, std::unique_ptr<resource>> stats_type;

    stats_type &g_store() {
        static stats_type s;
        return s;
    }


}


void proxy::stats::request(
    const fostlib::string &hash, const fostlib::url &uri
) {
    boost::upgrade_lock<boost::shared_mutex> read_lock(g_mutex);
    stats_type::iterator location(g_store().find(hash));
    if ( location == g_store().end() ) {
        /// Resource we've not seen before
        boost::upgrade_to_unique_lock<boost::shared_mutex>
            write_lock(read_lock);
        g_store().insert(std::make_pair(
            hash, std::unique_ptr<resource>(new resource(uri))));
        ++g_requests;
    } else {
        /// Already have the resource
        boost::mutex::scoped_lock lock(location->second->mutex);
        ++location->second->requests;
        boost::upgrade_to_unique_lock<boost::shared_mutex>
            write_lock(read_lock);
        ++g_requests;
    }
}

