/*
    Copyright 2008-2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/main>
#include <fost/threading>
#include <urlhandler.hpp>


using namespace fostlib;


namespace {
    setting< string > c_host( L"webserver", L"Server", L"Bind to", L"localhost" );
    setting< int > c_port( L"webserver", L"Server", L"Port", 8001 );
}


FSL_MAIN(
    L"webserver",
    L"Threaded HTTP server\nCopyright (c) 2009-2011, Felspar Co. Ltd."
)( fostlib::ostream &o, fostlib::arguments &args ) {
    // Bind server to host and port
    http::server server( host( args[1].value(c_host.value()) ), c_port.value() );
    o << L"Answering requests on http://" << server.binding() << L":" << server.port() << L"/" << std::endl;
    // Service requests
    server( urlhandler::service );
    // It will never get this far
    return 0;
}
