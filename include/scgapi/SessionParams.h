
#include "scgapi/Scg.h"

namespace scg_api {

class Scg;
class AuthInfo;
class Session;

namespace internals {

struct SessionParams {

    SessionParams(Scg& scg, const std::shared_ptr<AuthInfo>& authData)
    : parent{scg}, auth{authData} {}

    SessionParams(const SessionParams&) = default;

    Scg& parent;
    std::string url;
    std::shared_ptr<AuthInfo> auth;
};

std::shared_ptr<Session>CreateSession(const SessionParams& params,
                                      restc_cpp::Context& ctx);

} // namespace internals
} // namespace scgapi
