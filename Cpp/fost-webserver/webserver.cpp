/*
    Copyright 2008-2014 Felspar Co Ltd. http://support.felspar.com/
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
    setting< string > c_host(L"webserver", L"Server", L"Bind to", L"localhost");
    setting< int > c_port(L"webserver", L"Server", L"Port", 8001);
    setting< string > c_mime(L"webserver", L"Server", "MIME types",
            L"Configuration/mime-types.json");
    setting< json > c_load("webserver", "Server", "Load", json::array_t());
}


FSL_MAIN(
    L"webserver",
    L"Threaded HTTP server\nCopyright (c) 2002-2011, Felspar Co. Ltd."
)( fostlib::ostream &o, fostlib::arguments &args ) {
    // Process command line arguments last
    args.commandSwitch("p", c_port.section(), c_port.name());
    args.commandSwitch("h", c_port.section(), c_port.name());
    args.commandSwitch("m", c_mime.section(), c_mime.name());

    // Load MIME types
    urlhandler::load_mime_configuration(c_mime.value());

    // Load any shared objects
    fostlib::json so(c_load.value());
    std::vector< boost::shared_ptr<fostlib::dynlib> > dynlibs;
    for ( fostlib::json::const_iterator p(so.begin()); p != so.end(); ++p )
        dynlibs.push_back(boost::shared_ptr<fostlib::dynlib>(
            new dynlib(fostlib::coerce<fostlib::string>(*p))));

    // Bind server to host and port
    http::server server(host(c_port.value()));
    o << L"Answering requests on "
        L"http://" << server.binding() << L":" << server.port() << L"/" << std::endl;

    // Service requests
    server( urlhandler::service );

    // It will never get this far
    return 0;
}
