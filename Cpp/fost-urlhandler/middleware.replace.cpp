/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class middleware_template final : public fostlib::urlhandler::view {
      public:
        middleware_template() : view("fost.middleware.replace") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            auto const wrapped = execute(configuration, path, req, h);
            if (configuration.has_key("replace") /* TODO check media type */) {
                fostlib::string text = wrapped.first->body_as_string();
                for (auto const &[str, with] :
                     configuration["replace"].object()) {
                    text = fostlib::replace_all(
                            text, str, fostlib::coerce<f5::u8view>(with));
                }
                return {boost::make_shared<fostlib::text_body>(
                                text, wrapped.first->headers()),
                        wrapped.second};
            } else {
                return wrapped;
            }
        }
    } c_middleware_replace;


}
