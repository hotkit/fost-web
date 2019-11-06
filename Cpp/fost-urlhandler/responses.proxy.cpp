/**
    Copyright 2014-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/web-proxy.hpp>
#include <fost/log>
#include <fost/timer>


namespace {


    /**
        The transparent proxy doesn't change the request or the
        server response. It's used in situations where the web server
        is intended to intercept a request and route it on as if it had
        handled the request itself. This is normally done with the co-operation
        of the upstream host.
     */
    const class transparent final : public fostlib::web_proxy::base {
      public:
        transparent() : base("fost.proxy.transparent") {}

    } c_transparent;


}


/**
 * ## `fostlib::web_proxy::base`
 */


std::pair<boost::shared_ptr<fostlib::mime>, int>
        fostlib::web_proxy::base::operator()(
                json const &configuration,
                string const &path,
                http::server::request &request,
                host const &host) const {
    timer time;
    auto info = log::info(fostlib::c_fost_web_urlhandler);
    info("id", fostlib::guid());
    info("request", "path", "full", request.file_spec());
    info("request", "path", "outstanding", path);

    url const base{coerce<string>(configuration["base"])};
    auto const location = upstream(configuration, base, path, request, host);
    info("proxy", "url", location);
    info("proxy", "headers", request.data()->headers());

    http::user_agent ua(base);
    http::user_agent::request proxy(request.method(), location, request.data());
    auto response = ua(proxy);
    auto body{response->body()};
    info("repsonse", "status", response->status());
    info("time", time.seconds());

    return std::make_pair(body, response->status());
}


fostlib::url fostlib::web_proxy::base::upstream(
        json const &configuration,
        url const &base,
        string const &path,
        http::server::request &request,
        host const &host) const {
    url location{base, path};
    location.query(request.query_string());
    return location;
}
