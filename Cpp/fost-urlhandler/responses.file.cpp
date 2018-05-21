/*
    Copyright 2018 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class servefile : public fostlib::urlhandler::view {
    public:
        servefile()
        : view("fost.view.file") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &configuration,
            const fostlib::string &path,
            fostlib::http::server::request &req,
            const fostlib::host &h
        ) const {
            return fostlib::urlhandler::serve_file(
                configuration, req,
                fostlib::coerce<boost::filesystem::path>(configuration));
        }
    } c_servefile;


}

