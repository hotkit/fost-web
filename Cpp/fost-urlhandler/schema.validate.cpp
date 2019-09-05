/**
    Copyright 2018-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>

#include <fost/insert>
#include <f5/json/assertions.hpp>
#include <f5/json/schema.hpp>

const class schema_validation : public fostlib::urlhandler::view {
  public:
    schema_validation() : view("fost.schema.validate") {}

    std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &config,
            const fostlib::string &path,
            fostlib::http::server::request &req,
            const fostlib::host &host) const {
        if (!config.has_key("schema"))
            throw fostlib::exceptions::not_implemented(
                    "Configuration 'schema' must give a JSON Schema to");
        auto body_str = fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::utf8_string>(req.data()->data()));
        fostlib::json body = fostlib::json::parse(body_str);
        f5::json::schema s{fostlib::url{}, config["schema"]};
        auto valid = s.validate(body);
        if (not valid) {
            fostlib::json result;
            fostlib::insert(result, "schema", config["schema"]);
            auto e{(f5::json::validation::result::error)std::move(valid)};
            fostlib::insert(result, "error", "assertion", e.assertion);
            fostlib::insert(result, "error", "in-schema", e.spos);
            fostlib::insert(
                    result, "error", "in-data", fostlib::jcursor{} / e.dpos);
            boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    fostlib::json::unparse(result, true),
                    fostlib::mime::mime_headers(), L"application/json"));
            return std::make_pair(response, 422);
        }
        return execute(config["valid"], path, req, host);
    }
} c_schema_validation;

const fostlib::urlhandler::view &fostlib::urlhandler::schema_validation =
        c_schema_validation;
