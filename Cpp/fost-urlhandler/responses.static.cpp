/**
    Copyright 2011-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/insert>
#include <fost/push_back>
#include <fost/unicode>


namespace {


    const class directory : public fostlib::urlhandler::view {
      public:
        directory() : view{"fost.static.directory"} {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            fostlib::fs::path root(
                    fostlib::coerce<fostlib::fs::path>(configuration["root"]));
            fostlib::fs::path filename =
                    root / fostlib::coerce<fostlib::fs::path>(path);
            if (fostlib::fs::is_directory(filename)) {
                return serve_directory(
                        configuration, path, req, host, filename);
            } else {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "Told to serve a directory, but it turned out to be a "
                        "file");
            }
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> serve_directory(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host,
                const fostlib::fs::path &dirname) const {
            if (not static_cast<f5::u8view>(req.file_spec()).ends_with("/")) {
                fostlib::json redirect;
                fostlib::insert(redirect, "location", req.file_spec() + "/");
                return fostlib::urlhandler::response_302(
                        redirect, path, req, host);
            }
            auto filename = dirname;
            if (configuration.has_key("index")) {
                filename /= fostlib::coerce<fostlib::fs::path>(
                        configuration["index"]);
            } else {
                filename /= "index.html";
            }
            if (fostlib::fs::exists(filename)) {
                return fostlib::urlhandler::serve_file(
                        configuration, req, filename);
            }
            /// There is no index file in this directory
            return fostlib::urlhandler::response_403(
                    fostlib::json{}, path, req, host);
        }
    } c_directory;


    const class response_static : public fostlib::urlhandler::view {
      public:
        response_static() : view("fost.static") {}

        static bool allow_delete(const fostlib::json &conf) {
            return conf.has_key("verbs")
                    && fostlib::coerce<fostlib::nullable<bool>>(
                               conf["verbs"]["DELETE"])
                               .value_or(false);
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            fostlib::fs::path root(
                    fostlib::coerce<fostlib::fs::path>(configuration["root"]));
            fostlib::fs::path filename =
                    root / fostlib::coerce<fostlib::fs::path>(path);
            if (fostlib::fs::is_directory(filename)) {
                if (configuration.has_key("directory")) {
                    return execute(configuration["directory"], path, req, host);
                } else {
                    return c_directory.serve_directory(
                            configuration, path, req, host, filename);
                }
            } else {
                if (not fostlib::fs::exists(filename)) {
                    return fostlib::urlhandler::response_404(
                            fostlib::json(), path, req, host);
                }
            }
            if (req.method() == "GET" || req.method() == "HEAD") {
                return fostlib::urlhandler::serve_file(
                        configuration, req, filename);
            } else if (allow_delete(configuration) && req.method() == "DELETE") {
                fostlib::fs::remove(filename);
                boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                        L"<html><head><title>Resource deleted</title></head>"
                        L"<body><h1>Resource deleted</h1></body></html>",
                        fostlib::mime::mime_headers(), L"text/html"));
                return std::make_pair(response, 200);
            } else {
                fostlib::json c405;
                fostlib::push_back(c405, "allow", "GET");
                fostlib::push_back(c405, "allow", "HEAD");
                if (allow_delete(configuration))
                    fostlib::push_back(c405, "allow", "DELETE");
                return fostlib::urlhandler::response_405(c405, path, req, host);
            }
        }
    } c_response_static;


}


const fostlib::urlhandler::view &fostlib::urlhandler::static_server =
        c_response_static;
