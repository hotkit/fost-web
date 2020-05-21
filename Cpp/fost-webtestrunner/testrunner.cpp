/**
    Copyright 2011-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/urlhandler>


#ifdef FOST_OS_WINDOWS
#define FOST_WEBSERVER_TEST_DECLSPEC __declspec(dllexport)
#else
#define FOST_WEBSERVER_TEST_DECLSPEC
#endif


const class FOST_WEBSERVER_TEST_DECLSPEC testrunner :
public fostlib::urlhandler::view {
  public:
    testrunner() : view("fost.test.runner") {}

    std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &,
            const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &) const {
        boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                "<html><head><title>Web server tests</title></head>"
                "<body><h1>Web server tests</h1></body></html>",
                fostlib::mime::mime_headers(), "text/html"));
        return std::make_pair(response, 200);
    }
} c_testrunner;
