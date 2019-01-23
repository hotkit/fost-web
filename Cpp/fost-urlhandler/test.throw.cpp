/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>

#include <stdexcept>


namespace {


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
                if (config["exception"] == "std::logic_error") {
                    throw std::logic_error{static_cast<std::string>(message)};
                } else {
                    throw fostlib::exceptions::not_implemented(
                            __PRETTY_FUNCTION__);
                }
            }
        }

    } C_throw_exception;


}

const fostlib::urlhandler::view &fostlib::urlhandler::test_throw =
        C_throw_exception;
