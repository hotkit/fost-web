#include <fost/timer>
#include <fost/log>
#include <proxy/cache.hpp>
#include <proxy/views.hpp>
#include <boost/filesystem/fstream.hpp>


namespace {


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
            fostlib::timer started;
            fostlib::url base(
                configuration.has_key("origin") ?
                    fostlib::coerce<fostlib::string>(configuration["origin"]) :
                    "http://www.wallofsport.com/");
            fostlib::url location(base, request.file_spec());
            location.query() = request.query_string();
            auto info(std::move(
                fostlib::log::info()
                    ("id", fostlib::guid())
                    ("method", request.method())
                    ("location", location)
                    ("time", "begin", fostlib::timestamp::now())));

            fostlib::http::user_agent::request origin(
                request.method(), location);
            fostlib::json entry(proxy::db_entry(proxy::hash(origin)));
            info("cache", "entry", entry);
            if ( !entry.isnull() ) {
                fostlib::json variant(entry["variant"]
                    [fostlib::coerce<fostlib::string>(
                        proxy::variant(request.data()->headers()))]);
                if ( !variant.isnull() ) {
                    info("cache", "variant", variant);
                    info("cache", "hit", true);
                    boost::filesystem::wpath filename(
                        proxy::root() / fostlib::coerce<boost::filesystem::wpath>(
                                variant["pathname"]));
                    info("cache", "file", filename);
                    info("time", "end", fostlib::timestamp::now());
                    info("time", "duration", started.elapsed());
                    return std::make_pair(
                        boost::make_shared<fostlib::file_body>(filename,
                            fostlib::mime::mime_headers(),
                            fostlib::coerce<fostlib::string>(
                                variant["response"]["headers"]["Content-Type"])),
                        fostlib::coerce<int>(variant["response"]["status"]));
                } else {
                    info("cache", "hit", false);
                    info("request", "variant",
                        fostlib::coerce<fostlib::string>(
                            proxy::variant(request.data()->headers())));
                }
            }

            fostlib::http::user_agent ua(base);
            fostlib::http::user_agent::request ua_req("GET", location);
            if ( request.data()->headers().exists("Accept") ) {
                origin.headers().set("Accept",
                    request.data()->headers()["Accept"]);
            } else {
                origin.headers().set("Accept", "text/html");
            }
            std::auto_ptr< fostlib::http::user_agent::response >
                response = ua(origin);
            info("response", "status", response->status());
            info("response", "size", response->body()->data().size());

            boost::filesystem::wpath pathname =
                proxy::root() / proxy::save_entry(request, *response);
            if ( response->body()->data().size() ) {
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

            info("time", "end", fostlib::timestamp::now());
            info("time", "duration", started.elapsed());
            return std::make_pair(body, response->status());
        }

    } c_proxy_view;


}


const fostlib::urlhandler::view &proxy::view::c_proxy = c_proxy_view;

