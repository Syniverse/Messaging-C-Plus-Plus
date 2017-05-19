
#pragma once

#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <string>

#include <boost/optional.hpp>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "restc-cpp/restc-cpp.h"
#include "restc-cpp/RequestBuilder.h"
#include "restc-cpp/SerializeJson.h"
#include "restc-cpp/logging.h"

#include "scgapi/Scg.h"
#include "scgapi/Session.h"
#include "scgapi/AsyncForwardList.h"
#include "scgapi/AuthInfo.h"

namespace scg_api {

/*! \class ListParameters ResourceImpl.h scg_api/ResourceImpl.h
 *
 * Customize the result-set from queries
 */
struct ListParameters {
    /*! Start the listing at the # item in the result-set.
     *
     * This allows you to for example split a listing/processing
     * job of a huge result-set to several threads, where each
     * process a chunk of the full result-set.
     */
    std::int64_t start_offset = 0;

    /*! How many objects to return in response of one query.
     *
     * \note The server has a max value that cannot be overridden.
     */
    int page_size = 0;

    /*! Sort criteria */
    std::string sort;
};

/*! \internal */
struct GenericReply {
    std::string id;
};

/*! \internal */
struct GenericError {
    int error_code = 0;
    std::string error_description;
};

/*! \internal */
struct AccessToken {
    std::string accessToken;
    std::int64_t validityTime = 0;
};

/*! Base class for exceptions received from the SCG server */
struct ServerErrorException : public std::runtime_error
{
    ServerErrorException(GenericError&& err)
    : std::runtime_error(err.error_description)
    , server_error{std::move(err)}
    {}

    const GenericError server_error;
};

/*! Thrown when a requested object is not found on the SCG server */
struct NotFoundException : public ServerErrorException
{
    NotFoundException(GenericError&& err)
    : ServerErrorException{std::move(err)}
    {}
};

/*! Thrown when the authentication fails on the SCG server
 *
 * The SDK will by default try to recover from this error by
 * requesting a new auth token. If that is disabled or fails,
 * this exception is thrown.
 */
struct AuthenticationException : public ServerErrorException
{
    AuthenticationException(GenericError&& err)
    : ServerErrorException{std::move(err)}
    {}
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::GenericReply,
    (std::string, id))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::GenericError,
    (int, error_code)
    (std::string, error_description))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::AccessToken,
    (std::string, accessToken)
    (std::int64_t, validityTime))



namespace scg_api {

/*! Filters for List methods.
 *
 * This is a name = value type of list, where
 * the first string is the name, and the second is
 * the value to filter by.\
 */
using filter_t = std::map<std::string, std::string>;


/*! \internal
 * Utility methods that don't need to be template
 */
struct ResourceSupport {

    boost::optional<restc_cpp::Request::args_t>
    ToArgs(const filter_t *filter, const ListParameters *lp) {

        std::unique_ptr<filter_t> my_filter;
        if (lp && (lp->page_size
            || lp->start_offset
            || !lp->sort.empty())) {
            if (filter) {
                my_filter = std::make_unique<filter_t>(*filter);
            } else {
                my_filter = std::make_unique<filter_t>();
            }

            if (lp->page_size) {
                (*my_filter)["limit"] = std::to_string(lp->page_size);
            }

            if (!lp->sort.empty()) {
                (*my_filter)["sort"] = lp->sort;
            }

            filter = my_filter.get();
        }

        if (filter && !filter->empty()) {
            restc_cpp::Request::args_t args;
            for(const auto& it : *filter) {
                args.push_back({it.first, it.second});
            }

            return args;
        }

        return {};
    }

    const boost::optional<restc_cpp::Request::headers_t>
    ToHeaders(const AuthInfo& auth) {
        restc_cpp::Request::headers_t headers;
        const auto auth_conf = auth.GetConfig();

        if (auth_conf.token.empty()) {
            headers["Authorization"] = std::string("Bearer ") + auth_conf.token;
        }

        // For CI testing
        if (auth_conf.appid) {
            headers["int-appId"] = std::to_string(auth_conf.appid);
        }

        // For CI testing
        if (auth_conf.companyid) {
            headers["int-companyId"] = std::to_string(auth_conf.companyid);
        }

        // For CI testing
        if (!auth_conf.quotaplan.empty()) {
            headers["int-quota-plan"] = auth_conf.quotaplan;
        }

        // For CI testing
        if (!auth_conf.transactionid.empty()) {
            headers["int-txnId"] = auth_conf.transactionid;
        }

        headers["Accept"] = "*/*";
        headers["Accept-Encoding"] = "identity";
        //headers["Connection"] = "keep-alive";
        return headers;
    }

    static void SetOrReplaceArg(restc_cpp::Request::args_t& args,
                                const std::string& name,
                                const std::string& value) {
        bool found = false;
        for(auto& a : args) {
            if (a.name.compare(name) == 0) {
                a.value = value;
                found = true;
            }
        }

        if (!found) {
            args.push_back({name, value});
        }
    }

    void DealWithErrors(restc_cpp::Reply& reply) {
        auto response_code = reply.GetResponseCode();

        if (response_code < 300)
            return;

        if (response_code == 401) {
            GenericError error;
            error.error_code = response_code;
            error.error_description = reply.GetBodyAsString();
            throw AuthenticationException(std::move(error));
        }

        if (response_code >= 400) {
            GenericError error;
            restc_cpp::SerializeFromJson(error, reply);

            if (response_code == 404) {
                throw NotFoundException(std::move(error));
            }

            throw ServerErrorException(std::move(error));
        }

        std::stringstream msg;
        msg << "Request failed - got unexpected HTTP code "
            << response_code;
        throw std::runtime_error(msg.str());
    }
};

/*! \internal
 *
 * Generic implementation of the Resource class assigned to each
 * Data class.
 */
template <typename dataT>
class ResourceImpl : public ResourceSupport
{
public:
    using list_t = AsyncForwardList<dataT>;

    // The data type returned by Get()
    using data_ptr_t = std::unique_ptr<dataT>;

    Session& GetSession() { return session_; }

    std::weak_ptr<int> GetExists() {
        return exists_;
    }

protected:

    ResourceImpl(Session& session, std::string resource_url)
    : session_{session}, resource_url_{resource_url}, exists_{std::make_shared<int>(0)}
    {
    }

    virtual const restc_cpp::excluded_names_t *GetReadOnlyNames() {
        return nullptr;
    }

    virtual const restc_cpp::JsonFieldMapping *
    GetJsonFieldMapping() const noexcept {
        return nullptr;
    }

    auto DealWithErrorsAndAuth(restc_cpp::Request& req) {

        for(int retries = 0; ; ++retries) {
            auto reply = req.Execute(session_.GetContext());

            try {
                DealWithErrors(*reply);
                return reply;
            } catch(const AuthenticationException& ex) {
                if (retries >= session_.GetAuth().GetRetries()) {
                    RESTC_CPP_LOG_ERROR << "Auth failed: " << ex.what();
                    throw;
                }

                RESTC_CPP_LOG_DEBUG << "Auth failed: " << ex.what()
                    << ". Will try to get a new auth-token.";

                if (RefreshToken()) {
                    // Set the new token in the header and retry
                    auto props = std::make_shared<restc_cpp::Request::Properties>(
                        req.GetProperties());
                    props->headers["Authorization"] =
                        std::string("Bearer ") + session_.GetToken();

                    RESTC_CPP_LOG_DEBUG << "New token: '"
                        << session_.GetToken() << '\'';

                    req.SetProperties(props);
                }
            }
        }
    }

    bool RefreshToken() {
        auto& auth = session_.GetAuth();
        restc_cpp::Request::args_t args;
        args.emplace_back("consumerkey", auth.GetKey());
        args.emplace_back("consumersecret", auth.GetSecret());
        args.emplace_back("oldtoken", auth.GetToken());

        auto req = restc_cpp::Request::Create(
                "https://api.syniverse.com/saop-rest-data/v1/apptoken-refresh",
                restc_cpp::Request::Type::GET,
                session_.GetParent().GetRestClient(),
                nullptr,
                args);

        auto reply = req->Execute(session_.GetContext());
        DealWithErrors(*reply);
        AccessToken access_token;
        restc_cpp::SerializeFromJson(access_token, *reply);

        if (!access_token.accessToken.empty()) {
            auth.SetToken(access_token.accessToken);
            return true;
        }

        return false;
    }


    list_t List_(const filter_t *filter = nullptr,
                 const ListParameters *lp = nullptr) {

        auto args = ToArgs(filter, lp);
        auto headers = ToHeaders(session_.GetAuth());

        std::int64_t start_offset = 0;
        if (lp) {
            start_offset = lp->start_offset;
        }

        return list_t{[this, headers, args] (int64_t offset) mutable {

            static const std::string offset_name = "offset";

            if (offset) {
                const auto offset_value = std::to_string(offset);
                if (!args) {
                    args = restc_cpp::Request::args_t();
                }
                SetOrReplaceArg(*args, offset_name, offset_value);
            }

            auto req = restc_cpp::Request::Create(
                resource_url_,
                restc_cpp::Request::Type::GET,
                session_.GetParent().GetRestClient(),
                {}, // body
                args,
                headers);

            auto reply = DealWithErrorsAndAuth(*req);

            auto rval = std::make_unique<typename list_t::list_return_mappert_t>();
            restc_cpp::SerializeFromJson(*rval, *reply, GetJsonFieldMapping());

            for(auto& o : rval->list) {
                // Make operations directly on the object possible.
                o.SetResource(&static_cast<typename dataT::Resource&>(*this));
            }

            return std::move(rval);
        }, start_offset};
    }

    auto DoPostNoBody(const std::string& url,
                const restc_cpp::Request::args_t& args) {
        auto headers = ToHeaders(session_.GetAuth());
        auto req = restc_cpp::Request::Create(
                url,
                restc_cpp::Request::Type::POST,
                session_.GetParent().GetRestClient(),
                {}, // body
                args, // args
                headers);
        auto reply = DealWithErrorsAndAuth(*req);
        return std::move(reply);
    }

    template <typename objectT>
    auto DoPost(const objectT& object, const std::string& url) {
        
        const auto mappings = GetJsonFieldMapping();
        const auto ro_names = GetReadOnlyNames();

        auto request = restc_cpp::RequestBuilder(session_.GetContext())
            .Post(url)
            .AddHeaders(ToHeaders(session_.GetAuth()))
            .Data(object, mappings, ro_names)
            .Build();
        
        return DealWithErrorsAndAuth(*request);
    }

    template <typename createTypeT = dataT>
    auto Create_(const createTypeT& object) {

        auto reply = DoPost(object, resource_url_);

        GenericReply rval;
        restc_cpp::SerializeFromJson(rval, *reply);

        RESTC_CPP_LOG_DEBUG << "Created "
            << RESTC_CPP_TYPENAME(dataT)
            << " with id = " << rval.id << std::endl;

        return rval.id;
    }

    auto UploadFile_(const std::string& url,
                     const boost::filesystem::path& path,
                     const std::string& suggestedFileName,
                     const std::string& mimeType) {

//        auto body = std::make_unique<restc_cpp::Request::Body>(path);

        auto headers = ToHeaders(session_.GetAuth());
        if (mimeType.empty()) {
            headers.get()["Content-Type"] = "Application/octet-stream";
        } else {
            headers.get()["Content-Type"] = mimeType;
        }
        if (!suggestedFileName.empty()) {
            headers.get()["Content-Disposition"]
                = std::string("Attachment; filename=\"") + suggestedFileName + "\"";
        }
        
        auto request = restc_cpp::RequestBuilder(session_.GetContext())
            .Post(url)
            .AddHeaders(headers)
            .File(path)
            .Build();
        
        return DealWithErrorsAndAuth(*request);
    }

    void DownloadFile_(const std::string& url,
                      const boost::filesystem::path& path) {

        auto headers = ToHeaders(session_.GetAuth());
        auto req = restc_cpp::Request::Create(
                url,
                restc_cpp::Request::Type::GET,
                session_.GetParent().GetRestClient(),
                nullptr,
                {}, // args
                headers);
        auto reply = DealWithErrorsAndAuth(*req);

        std::ofstream file(path.string(),
                           std::ios::binary | std::ios::trunc);

        while(reply->MoreDataToRead()) {
            auto buffer = reply->GetSomeData();
            const char *ptr = boost::asio::buffer_cast<const char *>(buffer);
            const auto bytes = boost::asio::buffer_size(buffer);

            file.write(ptr, bytes);
            if (!file.good()) {
                throw std::runtime_error("File IO error on write");
            }
        }
    }

    void Update_(const dataT& object) {
        auto url = resource_url_ + "/" + object.id;
        DoPost(object, url);
    }

    void Delete_(const std::string& id) {
        DeleteUrl_(resource_url_ + "/" + id);
    }

    void DeleteUrl_(const std::string& url) {

        auto headers = ToHeaders(session_.GetAuth());
        auto req = restc_cpp::Request::Create(
                url,
                restc_cpp::Request::Type::DELETE,
                session_.GetParent().GetRestClient(),
                {},
                {}, // args
                headers);

        auto reply = req->Execute(session_.GetContext());

        // Skip over whatever the server is sending us
        while(reply->MoreDataToRead()) {
            reply->GetSomeData();
        }
    }

    data_ptr_t Get_(const std::string& id) {
        auto headers = ToHeaders(session_.GetAuth());
        auto req = restc_cpp::Request::Create(
                resource_url_ + "/" + id,
                restc_cpp::Request::Type::GET,
                session_.GetParent().GetRestClient(),
                {},
                {}, // args
                headers);

        auto reply = DealWithErrorsAndAuth(*req);
        auto object = std::make_unique<dataT>();
        restc_cpp::SerializeFromJson(*object, *reply, GetJsonFieldMapping());

        // Make operations directly on the object possible.
        object->SetResource(&
            static_cast<typename dataT::Resource&>(*this));

        return std::move(object);
    }

    const std::string& GetResourceUrl() const noexcept {
        return resource_url_;
    }

    std::shared_ptr<int> exists_;

private:
    Session& session_;
    const std::string resource_url_;
};

} // namespace

