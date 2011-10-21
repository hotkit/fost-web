/*
    Copyright 2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/urlhandler.hpp>
#include <fost/unicode>


namespace {
    typedef fostlib::setting<fostlib::json> setting_type;
    typedef boost::shared_ptr< setting_type > setting_ptr_type;
    typedef std::vector< setting_ptr_type > settings_database;
    settings_database &mime_database() {
        static settings_database db;
        return db;
    }
}


void fostlib::urlhandler::load_mime_configuration(
    const fostlib::string &filename
) {
    fostlib::string db_data(fostlib::utf::load_file(
        fostlib::coerce<boost::filesystem::wpath>(filename)));
    fostlib::json db_json(fostlib::json::parse(db_data));
    for ( fostlib::json::const_iterator jc(db_json.begin()); jc != db_json.end(); ++jc )
        mime_database().push_back(
            setting_ptr_type(new setting_type(
                filename, "MIME", fostlib::coerce<fostlib::string>(jc.key()), *jc)));
}

fostlib::string fostlib::urlhandler::mime_type(
        const boost::filesystem::wpath &filename
) {
    boost::filesystem::wpath extension = boost::filesystem::extension(filename);
    return fostlib::setting<fostlib::string>::value(
        fostlib::string("MIME"), fostlib::coerce<fostlib::string>(extension),
            fostlib::null).value("text/plain");
}
