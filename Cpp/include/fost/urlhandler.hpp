/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/internet>
#include <fost/http.server.hpp>


namespace fostlib {


    /// URL routing for requests
    namespace urlhandler {


        /// The host configuration
        extern const fostlib::setting< fostlib::json > c_hosts;
        /// The view configuration
        extern const fostlib::setting< fostlib::json > c_views;

        /// The prime routing for web sites
        bool service( fostlib::http::server::request &req );

        /// Load MIME configuration data from the specified file
        void load_mime_configuration(const fostlib::string &filename);

        /// Return the MIME type for the given file extension
        fostlib::string mime_type(const boost::filesystem::wpath &filename);

        /// A view class
        class view : boost::noncopyable {
            protected:
                /// The name of the configuration that the handler should tie to
                view(const fostlib::string &name);
                /// Allow sub-classing to work properly
                virtual ~view();

            public:
                /// Handle the request.
                virtual std::pair<boost::shared_ptr<fostlib::mime>, int >
                    operator () (const fostlib::json &configuration,
                        fostlib::http::server::request &request,
                        const fostlib::host &host) const = 0;

                /// Trace down the JSON for the view function and its configuration
                static std::pair<fostlib::string, fostlib::json>
                    find_view(const fostlib::string &view_name, const fostlib::json &view_config);

                /// Return the view that matches the provided name
                static const view &view_for(const fostlib::string &name);
        };


        /// Used to return a standard 404 to the user
        extern const view &response_404;

        /// Returns static files
        extern const view &static_server;

        /// Runs the view found at the location with the longest prefix
        extern const view &view_pathprefix;


    }


}
