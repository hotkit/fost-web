#include <fost/internet>


#pragma once


namespace proxy {


    namespace stats {


        /// Store that an inbound (proxy) request happened
        void request(const fostlib::string &hash, const fostlib::url &resource);

        /// Store a request to the origin resource
        void origin(const fostlib::string &hash, const fostlib::url &resource);
        /// Store a failure to fetch from the origin resource
        void failure(const fostlib::string &hash, const fostlib::url &resource);

        /// Store a cache hit against the requested resource
        void hit(const fostlib::string &hash, const fostlib::url &resource);
        /// Store a cache miss against the requested resource
        void miss(const fostlib::string &hash, const fostlib::url &resource);

        /// Load previous stats from the JSON structure
        void load(const fostlib::json &);
        /// Return a JSON structure describing the current stats
        fostlib::json current();


    }


}
