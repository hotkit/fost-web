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
    const struct transparent final : public fostlib::web_proxy::base {
        transparent() : base("fost.proxy.transparent") {}
    } c_transparent;

    /**
        The reverse proxy places itself between the user agent and
        the upstream. It will make sure that things like `Host` headers
        in the request and `Location` headers in the response are
        corrected so that all future requests also pass through it rather
        than the upstream web server.
     */
    const struct reverse final : public fostlib::web_proxy::base {
        reverse() : base("fost.proxy.reverse") {}
        /// Replace the Host header in the request
        fostlib::http::user_agent::request ua_request(
                fostlib::json const &configuration,
                fostlib::url location,
                fostlib::string const &path,
                fostlib::http::server::request &request,
                fostlib::host const &host) const override;
        /// Replace any `Location` header that goes to the upstream
        /// with the address used in the proxy request
        std::pair<boost::shared_ptr<fostlib::mime>, int>
                respond(fostlib::json const &configuration,
                        fostlib::url const &location,
                        fostlib::string const &path,
                        fostlib::http::server::request const &request,
                        fostlib::host const &host,
                        boost::shared_ptr<fostlib::mime> body,
                        int status) const override;
    } c_reverse;


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
    auto proxy = ua_request(configuration, location, path, request, host);
    auto response = ua(proxy);
    auto body{response->body()};
    info("response", "status", response->status());
    info("time", time.seconds());

    return respond(
            configuration, location, path, request, host, body,
            response->status());
}


fostlib::url fostlib::web_proxy::base::upstream(
        json const &configuration,
        url const &base,
        string const &path,
        http::server::request const &request,
        host const &host) const {
    url location{base, path};
    location.query(request.query_string());
    return location;
}


fostlib::http::user_agent::request fostlib::web_proxy::base::ua_request(
        json const &configuration,
        url location,
        string const &path,
        http::server::request &request,
        host const &host) const {
    return http::user_agent::request{
            request.method(), std::move(location), request.data()};
}


std::pair<boost::shared_ptr<fostlib::mime>, int>
        fostlib::web_proxy::base::respond(
                json const &configuration,
                fostlib::url const &location,
                string const &path,
                http::server::request const &request,
                host const &host,
                boost::shared_ptr<mime> body,
                int status) const {
    return {body, status};
}


/**
 * ## `reverse` proxy
 */


fostlib::http::user_agent::request reverse::ua_request(
        fostlib::json const &configuration,
        fostlib::url location,
        fostlib::string const &path,
        fostlib::http::server::request &request,
        fostlib::host const &host) const {
    request.headers().set("Host", location.server().name());
    return fostlib::http::user_agent::request{
            request.method(), std::move(location), request.data()};
}


std::pair<boost::shared_ptr<fostlib::mime>, int> reverse::respond(
        fostlib::json const &configuration,
        fostlib::url const &location,
        fostlib::string const &path,
        fostlib::http::server::request const &request,
        fostlib::host const &host,
        boost::shared_ptr<fostlib::mime> body,
        int status) const {
    if (body->headers().exists("Location")
        && configuration.has_key("Location")) {
        fostlib::url destination{location, body->headers()["Location"].value()};
        if (destination.server().name() == location.server().name()
            && destination.server().service() == location.server().service()) {
            fostlib::url loc_header{fostlib::coerce<fostlib::string>(
                    configuration["Location"])};
            fostlib::url new_location{
                    loc_header, loc_header.pathspec() + destination.pathspec()};
            new_location.query(destination.query());
            body->headers().set(
                    "Location", fostlib::coerce<fostlib::string>(new_location));
        }
    }
    return {body, status};
}
