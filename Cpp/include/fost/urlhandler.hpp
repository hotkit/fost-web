/**
    Copyright 2011-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once
#ifndef FOST_URLHANDLER_HPP
#define FOST_URLHANDLER_HPP


#include <fost/internet>
#include <fost/http.server.hpp>


namespace fostlib {


    extern const module c_fost_web;
    extern const module c_fost_web_urlhandler;


    /// URL routing for requests
    namespace urlhandler {


        /// The host configuration
        FOST_URLHANDLER_DECLSPEC
        extern const fostlib::setting<fostlib::json> c_hosts;
        /// The view configuration
        FOST_URLHANDLER_DECLSPEC
        extern const fostlib::setting<fostlib::json> c_views;

        /// The prime routing for web sites
        FOST_URLHANDLER_DECLSPEC
        bool service(fostlib::http::server::request &req);

        /// Load MIME configuration data from the specified file
        FOST_URLHANDLER_DECLSPEC
        void load_mime_configuration(const fostlib::string &filename);

        /// Return the MIME type for the given file extension
        FOST_URLHANDLER_DECLSPEC
        fostlib::string mime_type(const fostlib::fs::path &filename);

        /// A view class
        class FOST_URLHANDLER_DECLSPEC view : boost::noncopyable {
          protected:
            /// The name of the configuration that the handler should tie to
            view(const fostlib::string &name);
            /// Allow sub-classing to work properly
            virtual ~view();

          public:
            /// Handle the request.
            virtual std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                    const fostlib::json &configuration,
                    const fostlib::string &path,
                    fostlib::http::server::request &request,
                    const fostlib::host &host) const = 0;

            /// Trace down the JSON for the view function and its configuration
            static std::pair<fostlib::string, fostlib::json> find_view(
                    const fostlib::string &view_name,
                    const fostlib::json &view_config = fostlib::json());

            /// Return the view that matches the provided name
            static const view &view_for(const fostlib::string &name);

            /// Execute a subview
            static std::pair<boost::shared_ptr<fostlib::mime>, int>
                    execute(const fostlib::json &configuration,
                            const fostlib::string &path,
                            fostlib::http::server::request &request,
                            const fostlib::host &host);
        };

        /// Serves a file from disk providing etag, modified etc. and
        /// properly handling `If-None-Match` headers.
        std::pair<boost::shared_ptr<fostlib::mime>, int> serve_file(
                const fostlib::json &configuration,
                fostlib::http::server::request &req,
                const fostlib::fs::path &filename);


        /// ## Test views

        /// Throw an exception
        FOST_URLHANDLER_DECLSPEC
        extern const view &test_throw;

        /// ## Control flow views

        /// Routing to new views based on status condition
        FOST_URLHANDLER_DECLSPEC
        extern const view &control_status_condition;
        /// Catching exceptions and routing to different views
        FOST_URLHANDLER_DECLSPEC
        extern const view &control_exception_catch;


        /// Runs the view found at the location with the longest prefix
        FOST_URLHANDLER_DECLSPEC
        extern const view &view_pathprefix;
        /// Serves the view found based on an exact match of the URL
        FOST_URLHANDLER_DECLSPEC
        extern const view &view_pathname;


        /// ## Middleware

        /// Log the request and results
        FOST_URLHANDLER_DECLSPEC
        extern const view &middleware_logging;
        /// Alter the requests parameters before processing
        FOST_URLHANDLER_DECLSPEC
        extern const view &middleware_request;
        /// Wrap a template around a response
        FOST_URLHANDLER_DECLSPEC
        extern const view &middleware_template;

        /// request body JSON Schema validation
        FOST_URLHANDLER_DECLSPEC
        extern const view &schema_validation;

        /// match with path
        FOST_URLHANDLER_DECLSPEC
        extern const view &view_matcher;

        /// Generic response handler
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_generic;

        /// ## Standard responses

        /// Return a 200 response to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_200;
        /// Used to return a standard 301 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_301;
        /// Used to return a standard 302 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_302;
        /// Used to return a standard 303 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_303;
        /// Used to return a standard 403 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_401;
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_403;
        /// Used to return a standard 404 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_404;
        /// Used to return a standard 405 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_405;
        /// Used to return a standard 410 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_410;
        /// Used to return a standard 412 to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_412;
        /// Used to return a standard 500 (internal server error) to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_500;
        /// Used to return a standard 501 (not implemented) to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_501;
        /// Used to return a standard 503 (service unavailable) to the user
        FOST_URLHANDLER_DECLSPEC
        extern const view &response_503;


        /// ## Serving files

        /// Returns static files
        FOST_URLHANDLER_DECLSPEC
        extern const view &static_server;


    }


}


#endif // FOST_URLHANDLER_HPP
