/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>
#include <fost/threading>


namespace {
    typedef fostlib::threadsafe_store<urlhandler::view*> view_store;
    view_store &views() {
        static view_store store;
        return store;
    }
}

urlhandler::view::view(const fostlib::string &name) {
    views().add(name, this);
}

urlhandler::view::~view() {
    // Allow the view to leak as they'll only be removed on termination
}


const urlhandler::view &urlhandler::view::view_for(const fostlib::string &name) {
    view_store::found_t found(views().find(name));
    if ( found.size() == 1 )
        return *found[0];
    else
        throw fostlib::exceptions::not_implemented(
            "urlhandler::view::view_for(const fostlib::string &)",
            "Where zero or more than 1 views are found");
}


const class error_404 : urlhandler::view {
    public:
        error_404()
        : view("404") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            boost::shared_ptr<fostlib::mime> response(
                    new fostlib::text_body(
                        L"<html><body>Resource not found</body></html>",
                        fostlib::mime::mime_headers(), L"text/html" ));
            return std::make_pair(response, 404);
        }
} c_error_404;
