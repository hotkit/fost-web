/**
    Copyright 2011-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/unicode>


namespace {
    using setting_type = fostlib::setting<fostlib::json>;
    using setting_ptr_type = std::shared_ptr<setting_type>;
    using settings_database = std::vector<setting_ptr_type>;
    settings_database &mime_database() {
        static settings_database db;
        return db;
    }
}


void fostlib::urlhandler::load_mime_configuration(
        const fostlib::string &filename) {
    fostlib::string db_data(fostlib::utf::load_file(
            fostlib::coerce<fostlib::fs::wpath>(filename)));
    fostlib::json db_json(fostlib::json::parse(db_data));
    for (fostlib::json::const_iterator jc(db_json.begin()); jc != db_json.end();
         ++jc) {
        mime_database().push_back(std::make_shared<setting_type>(
                filename, "MIME", fostlib::coerce<fostlib::string>(jc.key()),
                *jc));
    }
}

fostlib::string
        fostlib::urlhandler::mime_type(const fostlib::fs::wpath &filename) {
    fostlib::fs::wpath extension = fostlib::fs::extension(filename);
    return fostlib::setting<fostlib::string>::value(
                   fostlib::string("MIME"),
                   fostlib::coerce<fostlib::string>(extension), fostlib::null)
            .value_or("text/plain");
}
