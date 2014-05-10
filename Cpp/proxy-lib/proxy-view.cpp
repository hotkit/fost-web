#include <proxy/views.hpp>


namespace {


    const class proxy_view : public fostlib::urlhandler::view {
    public:
        proxy_view()
        : view("proxy") {
        }


        std::pair<boost::shared_ptr<fostlib::mime>, int >
            operator () (const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &request,
                const fostlib::host &host) const {
            throw fostlib::exceptions::not_implemented("proxy view");
        }

    } c_proxy_view;


}


const fostlib::urlhandler::view &proxy::view::c_proxy = c_proxy_view;

