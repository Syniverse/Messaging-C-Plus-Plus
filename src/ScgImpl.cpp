
#include <iostream>
#include <functional>

#include "restc-cpp/logging.h"

#include "scgapi/Scg.h"
#include "scg_api_internals.h"

using namespace std;
using namespace restc_cpp;

namespace scg_api {

class ScgImpl : public Scg {
public:

    ScgImpl() {
        rest_client_ = RestClient::Create();
    }

    ScgImpl(const Request::Properties& properties) {
        rest_client_ = RestClient::Create(properties);
    }

    std::future<void>
    Connect(const string& url,
            const std::shared_ptr<AuthInfo>& auth,
            fn_t fn) override {
        internals::SessionParams sp{*this, auth};
        sp.url = url;

        auto promise = make_shared<std::promise<void>>();

        rest_client_->Process(std::bind(
            &ScgImpl::Process, this, std::placeholders::_1,
            sp, fn, promise));

        return promise->get_future();
    }

    RestClient& GetRestClient() override {
        assert(rest_client_);
        return *rest_client_;
    }

private:
    void Process(Context& ctx,
                 const internals::SessionParams& sp,
                 const fn_t& fn,
                 const std::shared_ptr<std::promise<void>>& promise) {

        auto session = internals::CreateSession(sp, ctx);

        try {
            fn(*session);
        } catch (const std::exception& ex) {
            RESTC_CPP_LOG_ERROR << "Process: Caught exception: " << ex.what() << endl;
            promise->set_exception(std::current_exception());
            return;
        }

        promise->set_value();
    }

    std::unique_ptr<RestClient> rest_client_;
};


std::shared_ptr< scg_api::Scg > scg_api::Scg::Create(
    const restc_cpp::Request::Properties& properties)
{
    return make_shared<ScgImpl>(properties);
}

std::shared_ptr< scg_api::Scg > scg_api::Scg::Create()
{
    return make_shared<ScgImpl>();
}


} // namespace scg_api
