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
