/*
    Copyright 2008-2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/dynlib>
#include <fost/log>
#include <fost/main>
#include <fost/threading>
#include <fost/unicode>
#include <fost/urlhandler>


using namespace fostlib;


namespace {
    setting< string > c_host( L"webserver", L"Server", L"Bind to", L"localhost" );
    setting< int > c_port( L"webserver", L"Server", L"Port", 8001 );
}


FSL_MAIN(
    L"webserver",
    L"Threaded HTTP server\nCopyright (c) 2002-2011, Felspar Co. Ltd."
)( fostlib::ostream &o, fostlib::arguments &args ) {
    // Load MIME types
    urlhandler::load_mime_configuration("Configuration/mime-types.json");

    // Load the configuration
    boost::filesystem::wpath configuration_file(
        fostlib::coerce<boost::filesystem::wpath>(args[1].value("webserver.json")));
    fostlib::string configuration_data(fostlib::utf::load_file(configuration_file, "{}"));
    fostlib::json configuration_json(fostlib::json::parse(configuration_data));

    // Set up the settings
    const fostlib::setting<fostlib::json>
        host_configuration(fostlib::coerce<fostlib::string>(configuration_file),
             "webserver", "hosts",
            configuration_json["webserver"]["hosts"]),
        view_configuration(fostlib::coerce<fostlib::string>(configuration_file),
             "webserver", "views",
            configuration_json["webserver"]["views"]);

    // Load any shared objects
    fostlib::json so(configuration_json["webserver"]["load"]);
    std::vector< boost::shared_ptr<fostlib::dynlib> > dynlibs;
    for ( fostlib::json::const_iterator p(so.begin()); p != so.end(); ++p )
        dynlibs.push_back(boost::shared_ptr<fostlib::dynlib>(
            new dynlib(fostlib::coerce<fostlib::string>(*p))));

    // Bind server to host and port
    args.commandSwitch("p", c_port.section(), c_port.name());
    http::server server( host( args[2].value(c_host.value()) ), c_port.value() );
    o << L"Answering requests on "
        L"http://" << server.binding() << L":" << server.port() << L"/" << std::endl;

    // Service requests
    server( urlhandler::service );

    // It will never get this far
    return 0;
}
