/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class request_middleware : public fostlib::urlhandler::view {
      public:
        request_middleware() : view("fost.middleware.request") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            for (auto h = configuration["headers"].begin();
                 h != configuration["headers"].end(); ++h) {
                req.headers().set(
                        fostlib::coerce<fostlib::string>(h.key()), *h);
            }
            return execute(configuration, path, req, h);
        }
    } c_middleware_request;


}


const fostlib::urlhandler::view &fostlib::urlhandler::middleware_request =
        c_middleware_request;
