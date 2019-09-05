/**
    Copyright 2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/test-throw-view.hpp>
#include <fost/urlhandler.hpp>
#include <f5/threading/map.hpp>

#include <stdexcept>


namespace {


    auto const g_pthrowers = []() {
        auto map = std::make_unique<f5::tsmap<
                fostlib::string, std::function<void(fostlib::string)>>>();
        map->emplace_if_not_found("std::logic_error", [](fostlib::string msg) {
            throw std::logic_error{static_cast<std::string>(msg)};
        });
        return map;
    }();


    const class throw_exception final : public fostlib::urlhandler::view {
      public:
        throw_exception() : view("test.throw") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const override {
            if (config.isnull()) {
                throw std::invalid_argument{
                        "Null configuration given to test.throw"};
            } else {
                fostlib::string const message =
                        fostlib::coerce<std::optional<f5::u8view>>(
                                config["message"])
                                .value_or(
                                        "Test exception message from "
                                        "test.throw");
                if (config.has_key("exception")) {
                    auto const exception_name =
                            fostlib::coerce<fostlib::string>(
                                    config["exception"]);
                    auto const pthrower = g_pthrowers->find(exception_name);
                    if (pthrower) {
                        pthrower(message);
                        throw fostlib::exceptions::not_implemented(
                                __PRETTY_FUNCTION__,
                                "Exception thrower didn't throw",
                                exception_name);
                    } else {
                        throw fostlib::exceptions::not_implemented(
                                __PRETTY_FUNCTION__, "Exception name not found",
                                exception_name);
                    }
                } else {
                    throw fostlib::exceptions::not_implemented(
                            __PRETTY_FUNCTION__,
                            "No exception name given to be thrown");
                }
            }
        }

    } c_throw_exception;


}

const fostlib::urlhandler::view &fostlib::urlhandler::test_throw =
        c_throw_exception;


fostlib::urlhandler::test_throw_plugin::test_throw_plugin(
        fostlib::string n, test_throw_plugin_fn f)
: name{n} {
    g_pthrowers->emplace_if_not_found(std::move(n), std::move(f));
}


fostlib::urlhandler::test_throw_plugin::~test_throw_plugin() {
    g_pthrowers->remove(name);
}
