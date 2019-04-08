/**
    Copyright 2008-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fost/dynlib>
#include <fost/log>
#include <fost/main>
#include <fost/threading>
#include <fost/unicode>
#include <fost/urlhandler>


using namespace fostlib;


namespace {
    const setting<string>
            c_host("webserver.cpp", "webserver", "Bind to", "localhost");
    const setting<int> c_port("webserver.cpp", "webserver", "Port", 8001);
    const setting<string>
            c_mime("webserver.cpp",
                   "webserver",
                   "MIME types",
                   "Configuration/mime-types.json");
    const setting<json>
            c_load("webserver.cpp", "webserver", "Load", json::array_t());

    const setting<json> c_logger(
            "webserver.cpp", "webserver", "logging", fostlib::json(), true);
    // Take out the Fost logger configuration so we don't end up with both
    const setting<json> c_fost_logger(
            "webserver.cpp",
            "webserver",
            "Logging sinks",
            fostlib::json::parse("{\"sinks\":[]}"));
}


FSL_MAIN(
        L"webserver",
        L"Threaded HTTP server\nCopyright (C) 2002-2016, Felspar Co. Ltd.")
(fostlib::ostream &o, fostlib::arguments &args) {
    // Load the configuration files we've been given on the command line
    std::vector<fostlib::settings> configuration;
    configuration.reserve(args.size());
    for (std::size_t arg{1}; arg != args.size(); ++arg) {
        o << "Loading config " << json(args[arg].value());
        auto filename = fostlib::coerce<fostlib::fs::path>(args[arg].value());
        configuration.emplace_back(std::move(filename));
    }

    // Load any shared objects
    fostlib::json so(c_load.value());
    std::vector<std::shared_ptr<fostlib::dynlib>> dynlibs;
    for (fostlib::json::const_iterator p(so.begin()); p != so.end(); ++p) {
        o << "Loading code plugin " << *p;
        dynlibs.push_back(std::make_shared<fostlib::dynlib>(
                fostlib::coerce<fostlib::string>(*p)));
    }

    // Process command line arguments last
    args.commandSwitch("p", c_port.section(), c_port.name());
    args.commandSwitch("h", c_host.section(), c_host.name());
    args.commandSwitch("m", c_mime.section(), c_mime.name());

    // Set up the logging options
    std::unique_ptr<fostlib::log::global_sink_configuration> loggers;
    if (not c_logger.value().isnull() && c_logger.value().has_key("sinks")) {
        loggers = std::make_unique<fostlib::log::global_sink_configuration>(
                c_logger.value());
    }

    // Load MIME types
    urlhandler::load_mime_configuration(c_mime.value());

    // Bind server to host and port
    http::server server(host(c_host.value()), c_port.value());
    o << L"Answering requests on "
         L"http://"
      << server.binding() << L":" << server.port() << L"/" << std::endl;

    // Service requests
    server(urlhandler::service);

    // It will never get this far
    return 0;
}
