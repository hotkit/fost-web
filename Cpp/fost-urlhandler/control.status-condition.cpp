/**
    Copyright 2018-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>

#include <fost/insert>
#include <f5/json/assertions.hpp>
#include <f5/json/schema.hpp>


namespace {


    const class control_status_condition : public fostlib::urlhandler::view {
      public:
        control_status_condition() : view("fost.control.status-condition") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            auto if_response = execute(config["if"], path, req, host);
            auto if_status_code = if_response.second;
            fostlib::string then_view =
                    "then." + std::to_string(if_status_code);
            if (config.has_key(then_view)) {
                return execute(config[then_view], path, req, host);
            } else if (if_status_code < 400) {
                if (config.has_key("then")) {
                    return execute(config["then"], path, req, host);
                }
                return if_response;
            }
            if (config.has_key("else")) {
                return execute(config["else"], path, req, host);
            }
            return if_response;
        }
    } c_control_status_condition;


}
