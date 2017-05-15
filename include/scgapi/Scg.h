#pragma once

/*! \mainpage SCG C++ API documentation
 *
 * The Syniverse Communication Gateway (SCG) API is a REST service
 * available from Syniverse. This C++ library is a simple wrapprer around
 * this REST service. For a full explanation about the properties
 * of the classes, please refer to
 *      https://api.syniverse.com/saop-store-ui/#/resource/admin/SCG_External_API
 *
 */
#ifndef SCG_H_
#define SCG_H_

#include <memory>
#include <string>
#include <future>
#include <mutex>

#include "restc-cpp/restc-cpp.h"
#include "scgapi/SessionParams.h"

namespace scg_api {

class Session;
class AuthInfo;


/*! \class Scg Scg.h "scg_api/Scg.h"
 *
 * Main entry point to the SCG API.
 *
 * In the C++ implementation of this API, each instance of this
 * class has it's private worker thread for asynchronous IO with
 * the server.
 */
class Scg {
protected:
    Scg() = default;

public:
    /*! Function callback to a coroutine that will execute commands
     * to the server. The function is implemented and looks just
     * like any other C++ function.
     *
     * \todo Document the async behaviour
     * \todo Document the iterator concept
     */
    using fn_t = std::function<void (Session& session)>;

    Scg(const Scg&) = delete;
    Scg(const Scg&&) = delete;

    virtual ~Scg() = default;

    void operator = (const Scg&) = delete;
    void operator = (const Scg&&) = delete;

    /*! Prepare a connection to a SCG API server
     *
     * The connection will be established later, when the first
     * REST request is sent to the server.
     *
     * \arg url Url to the API server. Typically
     *          "https://api.syniverse.com".
     * \arg auth Authentocation data anse a secret token you 
     *          obtain from the Syniverse
     *          developer portal.
     * \arg Function to call for the actual action.
     */
    virtual std::future<void> Connect(const std::string& url,
                 const std::shared_ptr<AuthInfo>& auth,
                 fn_t fn) = 0;

    template <typename T>
    std::future<T> ConnectT(const std::string& url,
            const std::shared_ptr<AuthInfo>& auth,
            std::function<T (Session&)> fn) {
        internals::SessionParams sp{*this, auth};
        sp.url = url;

        return GetRestClient().ProcessWithPromiseT<T>([fn, sp](restc_cpp::Context& ctx) {
            auto session = internals::CreateSession(sp, ctx);
            return fn(*session);
        });
    }

    /*! Return the instance of the restc_cpp::RestClient object
     * that is used by this instance. This in turn can give access to
     * logging functions, or the boost::asio ioservice used, in case
     * you want to post tasks between threads.
     */
    virtual restc_cpp::RestClient& GetRestClient() = 0;

    /*! Factory to get a new Sgc instance. */
    static std::shared_ptr<Scg> Create();
    static std::shared_ptr<Scg> Create(const restc_cpp::Request::Properties& properties);
};

} // namespace scg_api

#endif // SCG_H_
