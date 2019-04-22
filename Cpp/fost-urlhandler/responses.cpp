/**
    Copyright 2011-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    class response : public fostlib::urlhandler::view {
        int status;
        f5::u8string message;

      public:
        response()
        : view{"fost.response"},
          status{200},
          message{fostlib::http::server::status_text(200)} {}
        response(int s)
        : view{"fost.response." + std::to_string(s)},
          status{s},
          message{fostlib::http::server::status_text(s)} {}
        response(int s, f5::u8string m) : response(s) { message = m; }

        boost::shared_ptr<fostlib::mime> html(f5::u8string msg) const {
            f5::u8string resp_msg = "<html><head><title>" + msg
                    + "</title></head><body><h1>" + msg + "</h1></body></html>";
            boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    resp_msg, fostlib::mime::mime_headers(), "text/html"));
            return response;
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &,
                fostlib::http::server::request &,
                const fostlib::host &) const {
            int response_status{status};
            f5::u8string resp_msg{message};
            if (config.has_key("status")) {
                response_status = fostlib::coerce<int>(config["status"]);
            }
            if (config.has_key("message")) {
                resp_msg = fostlib::coerce<f5::u8string>(config["message"]);
            }

            return std::make_pair(html(resp_msg), response_status);
        }
    };


}


/**
 * ## List of generic responses
 */

const fostlib::urlhandler::view &fostlib::urlhandler::response_generic =
        response{};

const fostlib::urlhandler::view &fostlib::urlhandler::response_200 =
        response{200};

const fostlib::urlhandler::view &fostlib::urlhandler::response_403 =
        response{403};
const fostlib::urlhandler::view &fostlib::urlhandler::response_404 =
        response{404, "Resource not found"};
const fostlib::urlhandler::view &fostlib::urlhandler::response_410 =
        response{410};
const fostlib::urlhandler::view &fostlib::urlhandler::response_412 =
        response{412};

const fostlib::urlhandler::view &fostlib::urlhandler::response_500 =
        response{500};
const fostlib::urlhandler::view &fostlib::urlhandler::response_501 =
        response{501};
const fostlib::urlhandler::view &fostlib::urlhandler::response_503 =
        response{503};
