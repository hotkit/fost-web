/*
    Copyright 2011-2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class pathprefix : public fostlib::urlhandler::view {
    public:
        pathprefix()
        : view("fost.view.pathprefix") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &configuration,
            const fostlib::string &path,
            fostlib::http::server::request &req,
            const fostlib::host &h
        ) const {
            std::pair< std::size_t, fostlib::string> longest( 0, fostlib::string() );
            for ( fostlib::json::const_iterator p(configuration.begin());
                    p != configuration.end(); ++p ) {
                fostlib::string key = fostlib::coerce<fostlib::string>(p.key());
                std::size_t length = key.length();
                if ( path.substr(0, length) == key && length > longest.first )
                    longest = std::make_pair(length, key);
            }

            if ( configuration.has_key(longest.second) )
                return execute(configuration[longest.second], path.substr(longest.first), req, h);
            else
                return fostlib::urlhandler::response_404(fostlib::json(), path, req, h);
        }
} c_pathprefix;

const fostlib::urlhandler::view &fostlib::urlhandler::view_pathprefix = c_pathprefix;
