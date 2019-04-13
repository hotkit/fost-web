/**
    Copyright 2016-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    class response : public fostlib::urlhandler::view {
        int status;

      public:
        response(int s)
        : view{"fost.response." + std::to_string(s)}, status{s} {}

        f5::u8string html() const {
            return f5::u8string{"<html><head><title>"}
            + fostlib::http::server::status_text(status)
                    + "</title></head><body><h1>"
                    + fostlib::http::server::status_text(status)
                    + "</h1></body></html>";
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


const fostlib::urlhandler::view &fostlib::urlhandler::response_200 =
        response{200};

const fostlib::urlhandler::view &fostlib::urlhandler::response_403 =
        response{403};
