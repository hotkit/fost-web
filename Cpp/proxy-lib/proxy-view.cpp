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
            fostlib::url base("http://wos.appever.net:8000/");
            fostlib::url location(base, request.file_spec());
            auto info(
                fostlib::log::info()
                    ("id", fostlib::guid())
                    ("location", location)
                    ("time", "begin", fostlib::timestamp::now()));

            fostlib::json entry(proxy::db_entry(proxy::hash(request)));
            if ( !entry.isnull() ) {
                info("cache", "entry", entry);
                fostlib::json variant(entry["variant"]
                    [fostlib::coerce<fostlib::string>(
                        proxy::variant(request.data()->headers()))]);
                if ( !variant.isnull() ) {
                    info("cache", "variant", variant);
                    info("cache", "hit", true);
                    boost::filesystem::wpath filename(
                        fostlib::coerce<boost::filesystem::wpath>(
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
            std::auto_ptr< fostlib::http::user_agent::response >
                response = ua.get(location);
            info("response", "status", response->status());

            boost::filesystem::wpath pathname = proxy::save_entry(*response);
            info("response", "size", response->body()->data().size());
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

