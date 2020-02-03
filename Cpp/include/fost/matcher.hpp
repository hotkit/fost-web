/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once

#include <fost/core>


namespace fostlib {


    struct match {
        fostlib::json configuration;
        std::vector<fostlib::string> arguments;
    };


    using match_predicate = std::function<bool(match const &)>;
    fostlib::nullable<match>
            matcher(const fostlib::json &config, const fostlib::string &path, match_predicate = match_predicate{});


}
