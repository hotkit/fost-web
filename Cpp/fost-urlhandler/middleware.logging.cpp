/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>

#include <fost/log>

#include <chrono>


namespace fostlib {
    namespace log {
        FSL_DEFINE_LOGGING_LEVEL(ok, 0x600u);
    }
}


namespace {


    const fostlib::module c_rqlog(fostlib::c_fost_web_urlhandler, "logger");


    const class logging_middleware : public fostlib::urlhandler::view {
      public:
        logging_middleware() : view("fost.middleware.logging") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            auto addlog = [&req, started = std::chrono::steady_clock::now()](
                                  auto &logger) {
                logger("time", "seconds",
                       std::chrono::duration<double>(
                               std::chrono::steady_clock::now() - started)
                               .count())("request", "method", req.method());
                logger("request", "path", req.file_spec());
                logger("request", "bytes", req.data()->data().size());
                logger("request", "headers", req.headers());
                if (req.query_string().as_string()) {
                    logger("request", "query",
                           req.query_string().as_string().value());
                }
            };
            auto resplog = [&addlog](auto &logger, const auto &result) {
                logger("response", "status", result.second)(
                        "response", "headers", result.first->headers());
                addlog(logger);
            };
            try {
                auto result = execute(configuration, path, req, h);
                if (result.second < 400 || result.second == 401) {
                    auto logger = fostlib::log::ok(c_rqlog);
                    resplog(logger, result);
                } else {
                    auto logger = fostlib::log::warning(c_rqlog);
                    resplog(logger, result);
                }
                return result;
            } catch (std::exception &e) {
                auto logger = fostlib::log::error(c_rqlog);
                addlog(logger);
                logger("exception", "message",
                       e.what())("exception", "type", typeid(e).name());
                throw;
            } catch (...) {
                auto logger = fostlib::log::error(c_rqlog);
                addlog(logger);
                throw;
            }
        }
    } c_middleware_logging;


}


const fostlib::urlhandler::view &fostlib::urlhandler::middleware_logging =
        c_middleware_logging;
