/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/urlhandler.hpp>


const class middleware_template : public fostlib::urlhandler::view {
    public:
        middleware_template()
        : view("fost.middleware.template") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &configuration,
            fostlib::http::server::request &req,
            const fostlib::host &h
        ) const {
            fostlib::string view(fostlib::coerce<fostlib::string>(
                configuration["view"]));
            std::pair<fostlib::string, fostlib::json> view_fn = view::find_view(view);
            std::pair<boost::shared_ptr<fostlib::mime>, int>  wrapped(
                view_for(view_fn.first)(view_fn.second, req, h));
            if ( wrapped.second == 200 &&
                    wrapped.first->headers()["Content-Type"].value() == "text/html" ) {
                fostlib::string skin(
                    fostlib::utf::load_file(
                        fostlib::coerce<boost::filesystem::wpath>(configuration["template"])));
                fostlib::string content(
                    fostlib::coerce<fostlib::string>(*wrapped.first));

                std::pair<fostlib::string, fostlib::nullable<fostlib::string> > title(
                    fostlib::crack(content, "<title>", "</title>"));
                if ( !title.second.isnull() )
                    skin = replaceAll(skin, "{{ element:title }}",
                        L"<title>" + title.second.value() + L"</title>");

                std::pair<fostlib::string, fostlib::nullable<fostlib::string> > inner(
                    fostlib::crack(content, "<body>", "</body>"));
                if ( !inner.second.isnull() )
                    skin = replaceAll(skin, "{{ content:body }}", inner.second.value());

                boost::shared_ptr<fostlib::mime> response(
                        new fostlib::text_body(skin,
                            fostlib::mime::mime_headers(), L"text/html" ));
                return std::make_pair(response, 200);
            } else
                return wrapped;
        }
} c_middleware_template;

