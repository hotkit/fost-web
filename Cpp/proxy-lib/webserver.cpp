#include <fost/internet>
#include <fost/http.server.hpp>
#include <fost/log>
#include <proxy/webserver.hpp>
#include <proxy/views.hpp>


namespace {
    bool service(fostlib::http::server::request &req) {
        std::pair<boost::shared_ptr<fostlib::mime>, int > response =
            proxy::view::c_proxy(fostlib::json(),
                fostlib::coerce<fostlib::string>(req.file_spec()),
                req, fostlib::host());
        req(*response.first, response.second);
        return true;
    }

    fostlib::worker g_server;
    boost::shared_ptr< fostlib::detail::future_result< void > > g_running;
}


void proxy::start(const boost::filesystem::wpath &root) {
    g_running = g_server([]() {
        fostlib::http::server server(fostlib::host(0), 2555);
        server(service);
    });
    sleep(1);
}


void proxy::wait() {
}


void proxy::stop() {
}

