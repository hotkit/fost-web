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

    boost::mutex g_terminate_lock;
    bool g_terminate = false;
}


void proxy::start(const boost::filesystem::wpath &root) {
    g_running = g_server([]() {
        fostlib::http::server server(fostlib::host(0), 2555);
        server(service, []() {
            boost::mutex::scoped_lock lock(g_terminate_lock);
            return g_terminate;
        });
    });
}


void proxy::wait() {
    g_running->wait();
}


void proxy::stop() {
    { // Tell the server to stop
        boost::mutex::scoped_lock lock(g_terminate_lock);
        g_terminate = true;
    }
    // Tickle the port so it notices
    fostlib::network_connection tickle(fostlib::host("localhost"), 2555);
}

