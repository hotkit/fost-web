/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>

#include <fost/log>


namespace fostlib {
    namespace log {
        FSL_DEFINE_LOGGING_LEVEL(request, 0x700u);
    }
}


namespace {


    const class logging_middleware : public fostlib::urlhandler::view {
        public:
            logging_middleware()
            : view("fost.middleware.logging") {
            }

            std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h
            ) const {
                auto logger = fostlib::log::request(fostlib::c_fost_web_urlhandler);
                logger("", "HTTP Request");
                logger
                    ("request", "method", req.method())
                    ("request", "path", req.file_spec().underlying().underlying().c_str())
                    ("request", "bytes", req.data()->data().size());
                if ( not req.query_string().as_string().isnull() ) {
                    logger("request", "query", req.query_string().as_string().value().underlying().c_str());
                }
                auto result = execute(configuration, path, req, h);
                logger
                    ("request", "headers", req.headers())
                    ("response", "status", result.second);
                return result;
            }
    } c_middleware_logging;


}


const fostlib::urlhandler::view &fostlib::urlhandler::middleware_logging = c_middleware_logging;

