/**
    Copyright 2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/log>


namespace {


    const fostlib::module c_catch{fostlib::c_fost_web_urlhandler,
                                  "control.exception.catch"};


    const class catch_exception final : public fostlib::urlhandler::view {
      public:
        catch_exception() : view("control.exception.catch") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const override {
            try {
                return view::execute(config["try"], path, req, host);
            } catch (const std::exception &e) {
                auto logger{fostlib::log::debug(c_catch)};
                logger("caught", e.what());
                logger("typeid", typeid(e).name());
                if (config["catch"].isobject()) {
                    if (not config["catch"]["std::exception"].isnull()) {
                        return view::execute(
                                config["catch"]["std::exception"], path, req,
                                host);
                    }
                }
                throw;
            }
        }

    } c_catch_exception;


}

const fostlib::urlhandler::view &fostlib::urlhandler::control_exception_catch =
        c_catch_exception;
