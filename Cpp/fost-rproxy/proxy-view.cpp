/*
    Copyright 2014-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-rproxy.hpp"
#include <fost/timer>
#include <fost/log>
#include <fost/http-cache.hpp>
#include <fost/stats.hpp>
#include <fost/views.hpp>
#include <boost/filesystem/fstream.hpp>


const fostlib::module fostlib::c_fost_rproxy(fostlib::c_fost, "rproxy");


namespace {

    class capture_copy {
        bool capture;
        std::size_t level;
        fostlib::nliteral name;
        fostlib::json messages;
        fostlib::timer started;
        public:
            typedef fostlib::json result_type;

            capture_copy()
            : capture(true), level(0) {
                fostlib::insert(messages, "id", fostlib::guid());
                fostlib::insert(messages, "time", "began", fostlib::timestamp::now());
            }
            ~capture_copy() {
                capture = false;
                if ( level ) {
                    fostlib::insert(messages, "time", "end", fostlib::timestamp::now());
                    fostlib::insert(messages, "time", "duration", started.seconds());
                    fostlib::log::log(fostlib::c_fost_rproxy, level, name, messages);
                }
            }
            bool operator () (const fostlib::log::message &m) {
                using namespace fostlib;
                if ( m.level() == fostlib::log::g_stats_level ) {
                    return true;
                } else if ( capture && !m.body().isnull() ) {
                    if ( m.level() >  level ) {
                        level = m.level();
                        name = m.name();
                    }
                    fostlib::push_back(messages, "messages", coerce<json>(m));
                }
                return !capture;
            }
            fostlib::json operator() () {
                return messages;
            }
    };


    const class proxy_view : public fostlib::urlhandler::view {
    public:
        proxy_view()
        : view("proxy") {
        }


        std::pair<boost::shared_ptr<fostlib::mime>, int >
            operator () (const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &request,
                const fostlib::host &host) const {
            fostlib::log::scoped_sink< capture_copy > cc;

            fostlib::url base(fostlib::coerce<fostlib::string>(configuration["origin"]));
            fostlib::url location(base, request.file_spec());
            location.query() = request.query_string();
            fostlib::log::info(fostlib::c_fost_rproxy)
                ("request", "method", request.method())
                ("request", "path-spec",
                    fostlib::coerce<fostlib::nullable<boost::filesystem::wpath> >(
                        request.file_spec()))
                ("request", "query-string",
                    fostlib::coerce<fostlib::nullable<fostlib::string> >(
                        request.query_string().as_string()))
                ("request", "headers", request.data()->headers())
                ("origin", "url", location);

            fostlib::http::user_agent::request origin(
                request.method(), location);
            fostlib::string hash(fostlib::hash(origin));
            fostlib::log::stats(fostlib::c_fost_rproxy)
                ("key", hash)
                ("data", "resource", location)
                ("data", "accessed", fostlib::timestamp::now())
                ("add", "requests", 1);
            fostlib::json entry(fostlib::db_entry(hash));
            fostlib::log::info(fostlib::c_fost_rproxy)("cache", "entry", entry);
            if ( !entry.isnull() ) {
                fostlib::string vhash(
                        fostlib::variant(request.data()->headers()));
                fostlib::log::info(fostlib::c_fost_rproxy)("cache", "variant", vhash);
                fostlib::json variant(entry["variant"][vhash]);
                if ( !variant.isnull() ) {
                    fostlib::timediff ttl(
                        fostlib::coerce<fostlib::nullable<fostlib::timediff> >(
                            configuration["ttl"]).value_or(fostlib::hours(1)));
                    fostlib::log::info(fostlib::c_fost_rproxy)("cache", "ttl", ttl);
                    fostlib::timestamp expires(
                        fostlib::coerce<fostlib::timestamp>(variant["updated"]) + ttl);
                    fostlib::log::info(fostlib::c_fost_rproxy)("cache", "expires", expires);
                    if ( expires < fostlib::timestamp::now() ) {
                        try {
                            fostlib::log::info(fostlib::c_fost_rproxy)("cache", "miss", "expired");
                            fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                                ("add", "cache-misses-expired", 1);
                            return fetch_origin(hash, request, origin);
                        } catch ( fostlib::exceptions::exception &e ) {
                            fostlib::log::error(fostlib::c_fost_rproxy)
                                ("exception", e.what())
                                ("data", e.data());
                            fostlib::log::info(fostlib::c_fost_rproxy)("cache", "hit", true);
                            fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                                ("add", "cache-fallback-hits", 1);
                            return return_variant(
                                fostlib::coerce<fostlib::string>(entry["hash"]),
                                vhash, variant);
                        }
                    } else {
                        fostlib::log::info(fostlib::c_fost_rproxy)("cache", "hit", true);
                        fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                            ("add", "cache-hits", 1);
                        return return_variant(
                            fostlib::coerce<fostlib::string>(entry["hash"]),
                            vhash, variant);
                    }
                } else {
                    fostlib::log::info(fostlib::c_fost_rproxy)("cache", "miss", "variant not found");
                    fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                        ("add", "cache-misses-variant-not-found", 1);
                }
            }

            fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                ("add", "cache-misses-not-found", 1);
            return fetch_origin(hash, request, origin);
        }


        std::pair<boost::shared_ptr<fostlib::mime>, int >
                return_variant(
                    const fostlib::string &h, const fostlib::string &vh,
                    const fostlib::json &variant
                ) const {
            const boost::filesystem::wpath filename(
                fostlib::root() / fostlib::update_entry(h, vh));
            fostlib::log::debug(fostlib::c_fost_rproxy)
                ("cache", "file", filename);
            return std::make_pair(
                boost::shared_ptr<fostlib::file_body>(
                    new fostlib::file_body(filename,
                        fostlib::mime::mime_headers(),
                        fostlib::coerce<fostlib::string>(
                            variant["response"]["headers"]["Content-Type"]))),
                fostlib::coerce<int>(variant["response"]["status"]));
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int >
                fetch_origin(
                    const fostlib::string &hash,
                    fostlib::http::server::request &request,
                    fostlib::http::user_agent::request &ua_req
                ) const {
            fostlib::timer taken;
            try {
                std::pair<boost::shared_ptr<fostlib::mime>, int > result =
                    _fetch_origin(hash, request, ua_req);
                fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                    ("data", "origin-time-success", taken.seconds());
                return result;
            } catch ( fostlib::exceptions::socket_error & ) {
                fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                    ("data", "origin-time-failed", taken.seconds())
                    ("add", "origin-socket-error", 1);
                throw;
            } catch ( ... ) {
                fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                    ("data", "origin-time-failed", taken.seconds());
                throw;
            }
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int >
                _fetch_origin(
                    const fostlib::string &hash,
                    fostlib::http::server::request &request,
                    fostlib::http::user_agent::request &ua_req
                ) const {
            if ( request.data()->headers().exists("Accept") ) {
                ua_req.headers().set("Accept",
                    request.data()->headers()["Accept"]);
            } else {
                ua_req.headers().set("Accept", "text/html");
            }
            fostlib::log::info(fostlib::c_fost_rproxy)
                ("", "Fetching URL from origin")
                ("request", "url", ua_req.address())
                ("request", "headers", ua_req.data().headers());
            fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                ("add", "origin-requests", 1);
            fostlib::http::user_agent ua;
            auto response = ua(ua_req);
            fostlib::log::info(fostlib::c_fost_rproxy)
                ("response", "status", response->status())
                ("response", "size", response->body()->data().size());

            if ( response->status() >= 500 ) {
                fostlib::log::stats(fostlib::c_fost_rproxy)("key", hash)
                    ("add", "origin-500s", 1);
                throw fostlib::exceptions::not_implemented(
                    "No better handling for this status code "
                        "returned from the origin",
                    fostlib::coerce<fostlib::string>(response->status()));
            }

            const boost::filesystem::wpath pathname =
                fostlib::root() / fostlib::save_entry(request, *response);
            if ( response->body()->data().size() ) {
                fostlib::log::debug(fostlib::c_fost_rproxy)
                    ("saving", "pathname", pathname);
                boost::filesystem::ofstream(pathname,
                        std::ios_base::out | std::ios_base::binary).
                    write(reinterpret_cast<const char *>(
                            &response->body()->data()[0]),
                        response->body()->data().size());
            }

            fostlib::mime::mime_headers headers;
            headers.set("Content-Type", response->headers()["Content-Type"]);
            boost::shared_ptr<fostlib::mime> body(
                new fostlib::binary_body(response->body()->data(), headers));

            return std::make_pair(body, response->status());
        }

    } c_proxy_view;


}


const fostlib::urlhandler::view &fostlib::view::c_proxy = c_proxy_view;

