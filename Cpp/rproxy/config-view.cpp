#include <proxy/views.hpp>


namespace {


    const class config_view : public fostlib::urlhandler::view {
    public:
        config_view()
        : view("proxy.config") {
        }


        std::pair<boost::shared_ptr<fostlib::mime>, int >
            operator () (const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &request,
                const fostlib::host &host) const {
            throw fostlib::exceptions::not_implemented("config view");
        }

    } c_config_view;


}


const fostlib::urlhandler::view &proxy::view::c_config = c_config_view;

