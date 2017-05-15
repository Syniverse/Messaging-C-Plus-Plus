
#include "restc-cpp/restc-cpp.h"

#include "scgapi/Scg.h"
#include "scgapi/Session.h"
#include "scgapi/AuthInfo.h"
#include "scg_api_internals.h"

using namespace std;
using namespace restc_cpp;

namespace scg_api {

class SessionImpl : public Session {
public:

    SessionImpl(const internals::SessionParams& params,
                Context& ctx)
    : params_{params}, ctx_{ctx}
    {
    }

    Scg& GetParent() override { return params_.parent; }
    string GetToken() const override { return params_.auth->GetToken(); }
    const string& GetUrl() const override { return params_.url; }
    restc_cpp::Context& GetContext() override { return ctx_; }
    AuthInfo& GetAuth() override { return *params_.auth; }

private:
    internals::SessionParams params_;
    Context& ctx_;
};



namespace internals {

std::shared_ptr<Session> CreateSession(const SessionParams& params,
                                       Context& ctx) {
    return make_shared<SessionImpl>(params, ctx);
}

} // namespace internals
} // namespace scg_api
