
#pragma once

#ifndef SCGAIP_SESSION_H_
#define SCGAIP_SESSION_H_

#include <memory>
#include <string>

namespace scg_api {

class AuthInfo;

/*! Session handle for requests to the SCG API server.
 *
 */
class Session {
protected:
    Session() = default;

public:
    Session(const Session&) = delete;
    Session(const Session&&) = delete;

    virtual ~Session() = default;

    void operator = (const Session&) = delete;
    void operator = (const Session&&) = delete;

    virtual Scg& GetParent() = 0;
    virtual const std::string& GetUrl() const = 0;
    virtual std::string GetToken() const = 0;
    virtual restc_cpp::Context& GetContext() = 0;
    virtual AuthInfo& GetAuth() = 0;
};

} // namespace scg_api

#endif // SCGAIP_SESSION_H_
