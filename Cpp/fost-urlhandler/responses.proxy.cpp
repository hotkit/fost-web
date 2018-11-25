/**
    Copyright 2014-2018 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/log>
#include <fost/timer>


namespace {


    const class proxy_view final : public fostlib::urlhandler::view {
      public:
        proxy_view() : view("fost.proxy.transparent") {}


        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &request,
                const fostlib::host &host) const override {
            fostlib::timer time;
            auto info = fostlib::log::info(fostlib::c_fost_web_urlhandler);
            info("id", fostlib::guid())(
                    "request", "path", "full", request.file_spec())(
                    "request", "path", "outstanding", path);

            fostlib::url base(
                    fostlib::coerce<fostlib::string>(configuration["base"]));
            fostlib::url location(base, request.file_spec());
            location.query(request.query_string());
            info("proxy", "url", location);

            fostlib::http::user_agent ua(base);
            fostlib::http::user_agent::request proxy(
                    request.method(), location, request.data());
            auto response = ua(proxy);
            auto body{response->body()};
            info("repsonse", "status", response->status());
            info("time", time.seconds());

            return std::make_pair(body, response->status());
        }

    } c_proxy_view;


}
