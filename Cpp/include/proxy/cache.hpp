#include <fost/http>
#include <fost/http.server.hpp>


namespace proxy {


    /// Return the cache address hash for the request
    fostlib::hex_string hash(const fostlib::http::server::request &req);


}

