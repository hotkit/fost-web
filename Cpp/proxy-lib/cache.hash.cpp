#include <fost/crypto>
#include <proxy/cache.hpp>


fostlib::hex_string proxy::hash(const fostlib::http::server::request &req) {
    fostlib::digester md5(fostlib::md5);
    md5 << req.method() << fostlib::coerce<fostlib::string>(req.file_spec());
    return fostlib::coerce<fostlib::hex_string>(md5.digest());
}


fostlib::hex_string proxy::hash(
        const fostlib::http::user_agent::response &response) {
    fostlib::digester md5(fostlib::md5);
    md5 << response.method() <<
        fostlib::coerce<fostlib::string>(response.address().pathspec());
    return fostlib::coerce<fostlib::hex_string>(md5.digest());
}


fostlib::hex_string proxy::variant(
        const fostlib::mime::mime_headers &headers) {
    fostlib::digester md5(fostlib::md5);
    const fostlib::string newline("\n"), separator(": ");
    if ( headers.exists("Vary") ) {
        fostlib::split_type names(fostlib::split(headers["Vary"].value(), ","));
        for ( auto name : names ) {
            if ( headers.exists(name) ) {
                md5 << name << separator <<
                    fostlib::coerce<fostlib::string>(headers[name]) << newline;
            } else {
                md5 << newline;
            }
        }
    }
    return fostlib::coerce<fostlib::hex_string>(md5.digest());
}

