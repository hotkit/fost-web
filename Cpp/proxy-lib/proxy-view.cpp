#include <fost/timer>
#include <fost/log>
#include <proxy/cache.hpp>
#include <proxy/views.hpp>
#include <boost/filesystem/fstream.hpp>


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
                    fostlib::insert(messages, "time", "duration", started.elapsed());
                    fostlib::log::log(level, name, messages);
                }
            }
            bool operator () (const fostlib::log::message &m) {
                using namespace fostlib;
                if ( capture && !m.body().isnull() ) {
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
            fostlib::url base(
                configuration.has_key("origin") ?
                    fostlib::coerce<fostlib::string>(configuration["origin"]) :
                    "http://www.wallofsport.com/");
            fostlib::url location(base, request.file_spec());
            location.query() = request.query_string();
            auto info(std::move(
                fostlib::log::info()
                    ("method", request.method())
                    ("location", location)));

            fostlib::http::user_agent::request origin(
                request.method(), location);
            fostlib::json entry(proxy::db_entry(proxy::hash(origin)));
            info("cache", "entry", entry);
            if ( !entry.isnull() ) {
                fostlib::string vhash(fostlib::coerce<fostlib::string>(
                        proxy::variant(request.data()->headers())));
                info("cache", "variant", vhash);
                fostlib::json variant(entry["variant"][vhash]);
                if ( !variant.isnull() ) {
                    info("cache", "hit", true);
                    return return_variant(
                        fostlib::coerce<fostlib::string>(entry["hash"]),
                        vhash, variant);
                } else {
                    info("cache", "miss", "variant not found");
                }
            }

            return fetch_origin(request, origin);
        }


        std::pair<boost::shared_ptr<fostlib::mime>, int >
                return_variant(
                    const fostlib::string &h, const fostlib::string &vh,
                    const fostlib::json &variant
                ) const {
            const boost::filesystem::wpath filename(
                proxy::root() / proxy::update_entry(h, vh));
            fostlib::log::debug()
                ("cache", "file", filename);
            return std::make_pair(
                boost::make_shared<fostlib::file_body>(filename,
                    fostlib::mime::mime_headers(),
                    fostlib::coerce<fostlib::string>(
                        variant["response"]["headers"]["Content-Type"])),
                fostlib::coerce<int>(variant["response"]["status"]));
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int >
                fetch_origin(
                    fostlib::http::server::request &request,
                    fostlib::http::user_agent::request &ua_req
                ) const {
            if ( request.data()->headers().exists("Accept") ) {
                ua_req.headers().set("Accept",
                    request.data()->headers()["Accept"]);
            } else {
                ua_req.headers().set("Accept", "text/html");
            }
            fostlib::log::info()
                ("", "Fetching URL from origin")
                ("request", "url", ua_req.address())
                ("request", "headers", ua_req.data().headers());
            fostlib::http::user_agent ua;
            std::auto_ptr< fostlib::http::user_agent::response >
                response = ua(ua_req);
            fostlib::log::info()
                ("response", "status", response->status())
                ("response", "size", response->body()->data().size());

            const boost::filesystem::wpath pathname =
                proxy::root() / proxy::save_entry(request, *response);
            if ( response->body()->data().size() ) {
                fostlib::log::debug()
                    ("saving", "pathname", pathname);
                boost::filesystem::ofstream(pathname,
                        std::ios_base::out | std::ios_base::binary).
                    write(reinterpret_cast<const char *>(
                            response->body()->data().data()),
                        response->body()->data().size());
            }

            fostlib::mime::mime_headers headers;
            headers.set("Content-Type", response->headers()["Content-Type"]);
            boost::shared_ptr<fostlib::mime> body =
                boost::make_shared<fostlib::binary_body>(
                    response->body()->data(), headers);

            return std::make_pair(body, response->status());
        }

    } c_proxy_view;


}


const fostlib::urlhandler::view &proxy::view::c_proxy = c_proxy_view;

