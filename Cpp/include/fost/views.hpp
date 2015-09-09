/*
    Copyright 2014-2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/urlhandler>


namespace fostlib {


    const extern module c_fost_rproxy;


    namespace view {


        FOST_RPROXY_DECLSPEC
        extern const fostlib::urlhandler::view &c_config;
        FOST_RPROXY_DECLSPEC
        extern const fostlib::urlhandler::view &c_proxy;


    }


}

