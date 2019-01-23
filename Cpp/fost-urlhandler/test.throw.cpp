/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class throw_exception : public fostlib::urlhandler::view {
      public:
        throw_exception() : view("test.throw") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const override {
            throw fostlib::exceptions::not_implemented(__PRETTY_FUNCTION__);
        }

    } C_throw_exception;


}

const fostlib::urlhandler::view &fostlib::urlhandler::test_throw =
        C_throw_exception;
