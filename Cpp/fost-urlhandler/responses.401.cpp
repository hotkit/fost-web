/**
    Copyright 2012-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_401 : public fostlib::urlhandler::view {
  public:
    response_401() : view("fost.response.401") {}

    std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &config,
            const fostlib::string &path,
            fostlib::http::server::request &req,
            const fostlib::host &host) const {
        if (!config.has_key("schemes"))
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__,
                    "No 'schemes' key in the configuration so can't build "
                    "the required WWW-Authenticate header");
        fostlib::string schemes;
        for (fostlib::json::const_iterator m(config["schemes"].begin());
             m != config["schemes"].end(); ++m) {
            if (schemes.empty())
                schemes = fostlib::coerce<fostlib::string>(m.key());
            else
                schemes += ", " + fostlib::coerce<fostlib::string>(m.key());
        }
        boost::shared_ptr<fostlib::mime> response;
        if (config["view"].isnull()) {
            response.reset(new fostlib::text_body(
                    "<html><head><title>Not authorized</title></head>"
                    "<body><h1>Not authorized</h1></body></html>",
                    fostlib::mime::mime_headers(), "text/html"));
        } else {
            response = execute(config, path, req, host).first;
        }
        response->headers().set("WWW-Authenticate", schemes);
        return std::make_pair(response, 401);
    }
} c_response_401;
