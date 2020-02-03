/**
    Copyright 2019-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/fsigma.hpp>


namespace fostlib {


    /// Returns the base frame with the available preconditions
    fsigma::frame preconditions(
            const fostlib::http::server::request &,
            const std::vector<fostlib::string> &);


}
