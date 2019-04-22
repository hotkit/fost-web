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
        directory(f5::u8string name) : view{name} {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            if (not configuration.has_key("root")) {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "Must specify a root folder when serving a directory");
            }
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
            return listing(configuration, path, req, host, dirname);
        }

        virtual std::pair<boost::shared_ptr<fostlib::mime>, int>
                listing(const fostlib::json &configuration,
                        const fostlib::string &path,
                        fostlib::http::server::request &req,
                        const fostlib::host &host,
                        const fostlib::fs::path &) const {
            return fostlib::urlhandler::response_403(
                    fostlib::json{}, path, req, host);
        }
    } c_directory{"fost.static.directory"};


    const class directory_json : public directory {
      public:
        directory_json() : directory("fost.static.directory.json") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int>
                listing(const fostlib::json &configuration,
                        const fostlib::string &path,
                        fostlib::http::server::request &req,
                        const fostlib::host &host,
                        const fostlib::fs::path &directory) const {
            fostlib::json files = fostlib::json::object_t{};
            for (auto const &node :
                 fostlib::fs::directory_iterator(directory)) {
                fostlib::json entry;
                if (fostlib::fs::is_directory(node.path())) {
                    fostlib::insert(entry, "directory", true);
                    fostlib::insert(
                            entry, "path",
                            fostlib::coerce<fostlib::string>(
                                    node.path().filename())
                                    + "/");
                } else {
                    fostlib::insert(entry, "directory", false);
                    fostlib::insert(entry, "path", node.path().filename());
                }
                fostlib::insert(
                        files,
                        fostlib::coerce<fostlib::string>(node.path().filename()),
                        entry);
            }
            boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    fostlib::json::unparse(files, false),
                    fostlib::mime::mime_headers(), "application/json"));
            return std::make_pair(response, 200);
        }
    } c_directory_json;


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
