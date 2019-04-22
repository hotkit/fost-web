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

        f5::u8string html() const {
            return "<html><head><title>" + message + "</title></head><body><h1>"
                    + message + "</h1></body></html>";
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &,
                const fostlib::string &,
                fostlib::http::server::request &,
                const fostlib::host &) const {
            boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    html(), fostlib::mime::mime_headers(), "text/html"));
            return std::make_pair(response, status);
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
