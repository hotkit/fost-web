/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/log>


#pragma once


namespace fostlib {


    namespace log {


        /// Constant containing the capture log level
        const unsigned g_stats_level = 0x880u;


        /// Define a new logger level 'capture'
        FSL_DEFINE_LOGGING_LEVEL(stats, g_stats_level);


    }

}


namespace fostlib {


    namespace stats {


        void load(const fostlib::json &);


    }


}
