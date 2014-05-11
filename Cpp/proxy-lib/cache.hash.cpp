#include <fost/crypto>
#include <proxy/cache.hpp>


fostlib::hex_string proxy::hash(const fostlib::http::server::request &req) {
    fostlib::digester md5(fostlib::md5);
    md5 << req.method() << fostlib::coerce<fostlib::string>(req.file_spec());
    return fostlib::coerce<fostlib::hex_string>(md5.digest());
}

