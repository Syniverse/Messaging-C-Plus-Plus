#pragma once

#ifndef SCG_AUTH_INFO_H_
#define SCG_AUTH_INFO_H_

#include <memory>
#include <string>
#include <future>
#include <mutex>

#include <boost/fusion/adapted.hpp>

#include "restc-cpp/restc-cpp.h"

namespace scg_api {

/*! \class AuthInfo Scg.h "scg_api/Scg.h"
 *
 * Authentication data while using the API.
 *
 * You should have one instance of this class for each API
 * Application registration you use.
 *
 * The access-token may be updated with a new valid token if it
 * expires.
 *
 * All the methods for this class are thread-safe.
 *
 */
class AuthInfo {
public:
    struct Config {
        int retries = 3;
        std::string key;
        std::string secret;
        std::string token;

        // For internal use
        std::int64_t appid{};
        int companyid{};
        std::string quotaplan;
        std::string transactionid;
    };
    
    using ptr_t = std::shared_ptr<AuthInfo>;

    /*! Constructor
     *
     * \param consumerKey from SAOP store
     * \param consumerSecret from SAOP store
     * \param accessToken The current or last obtained Auth Token
     *          from the SAOP store. The token may be re-freshed
     *          behind the scenes if the numRetries value is > 0
     * \param numRetries Number of times to re-try refreshing the
     *          auth token before giving up. This value is used by
     *          behind the scenes by all the methods that communicate
     *          with the REST server. Set it to 0 to disable automatic
     *          re-fresh of the auth token.
     */
    AuthInfo(const std::string& consumerKey,
             const std::string& consumerSecret,
             const std::string& accessToken,
             int numRetries = 3)
    {
        config_.retries = numRetries;
        config_.key = consumerKey;
        config_.secret = consumerSecret;
        config_.token = accessToken;
    }
    
    AuthInfo(const boost::filesystem::path& jsonConfig);

    AuthInfo() = delete;

    /// Get the consumer key.
    const std::string& GetKey() const { return config_.key; }

    /// Get the consumer secret.
    const std::string& GetSecret() const { return config_.secret; }

    /*! Get the current access token.
     *
     * \note The SDK may refresh the token if it times out. The
     *          token returned can therefore be a previous seen token,
     *          or a new token.
     */
    std::string GetToken() const {
         std::lock_guard<std::mutex> lock{mutex_};
        return config_.token;
    }

    /*! Set Token.
     *
     * Sets the token to use.
     */
    void SetToken(std::string token) {
        std::lock_guard<std::mutex> lock{mutex_};
        config_.token = token;
    }

    /*! Get the number of times the SDK will retry authentication refresh.
     *
     * If the value is 0, no refresh will be attempted.
     */
    int GetRetries() const { return config_.retries; }

    // For internal use
    const Config& GetConfig() const { return config_; }

private:
    Config config_;
    mutable std::mutex mutex_;
};

} // namespace scg_api

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::AuthInfo::Config,
    (int, retries)
    (std::string, key)
    (std::string, secret)
    (std::string, token)
    (std::int64_t, appid)
    (int, companyid)
    (std::string, quotaplan)
    (std::string, transactionid)
)

#endif // SCG_AUTH_INFO_H_
