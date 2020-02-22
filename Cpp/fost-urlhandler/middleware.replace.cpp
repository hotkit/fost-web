/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/log>


namespace {


    fostlib::module const c_replace{fostlib::c_fost_web_urlhandler, "replace"};


    const class middleware_template final : public fostlib::urlhandler::view {
      public:
        middleware_template() : view("fost.middleware.replace") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            auto const wrapped = execute(configuration, path, req, h);
            auto const wrapped_content_type =
                    wrapped.first->headers()["Content-Type"].value();
            if (configuration.has_key("replace")
                && (wrapped_content_type == "text/html"
                    || wrapped_content_type == "application/javascript")) {
                fostlib::string text = wrapped.first->body_as_string();
                for (auto const &[str, with] :
                     configuration["replace"].object()) {
                    fostlib::json replacement{with};
                    if (with.isarray() && with.size() > 0) {
                        if (with[0] == "setting" && with.size() >= 3) {
                            auto const section =
                                    fostlib::coerce<fostlib::string>(with[1]);
                            auto const name =
                                    fostlib::coerce<fostlib::string>(with[2]);
                            if (with.size() == 4) {
                                replacement =
                                        fostlib::setting<fostlib::json>::value(
                                                section, name, with[3]);
                            } else {
                                replacement =
                                        fostlib::setting<fostlib::json>::value(
                                                section, name);
                            }
                        } else {
                            fostlib::log::warning(c_replace)(
                                    "", "Cannot understand replacement array")(
                                    "replacing", with);
                        }
                    } else if (with.isobject()) {
                        fostlib::log::warning(c_replace)(
                                "", "Cannot understand replacement object")(
                                "replacing", with);
                    }
                    if (not replacement.isatom()) {
                        text = fostlib::replace_all(
                                text, str,
                                fostlib::json::unparse(replacement, false));
                    } else {
                        text = fostlib::replace_all(
                                text, str,
                                fostlib::coerce<fostlib::string>(replacement));
                    }
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
