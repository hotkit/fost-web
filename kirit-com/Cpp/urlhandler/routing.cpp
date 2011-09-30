/*
    Copyright 2008-2011 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <urlhandler.hpp>


bool urlhandler::service( fostlib::http::server::request &req ) {
    fostlib::text_body response(
            L"<html><body>This <b>is</b> a response</body></html>",
            fostlib::mime::mime_headers(), L"text/html" );
    req( response );
    return true;
}
