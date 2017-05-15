#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class SenderIdType SenderIdType.h scg_api/SenderIdType.h
 *
 * This is an immutable resource that represents the list of
 * supported sender types that an be associated with a sender id.
 */
class SenderIdType : public BaseData
{
public:
    /// Unique id of the address type
    std::string id;
    /// Name of the address type
    std::string name;
    /// human readable description of the sender address type
    std::string description;
    /*! Message types allowed on the sender ids from of this type-
     *  SMS, MMS, EMAIL, etc
     */
    std::vector<std::string> capabilities;
    /*! Content type allowed on the sender id. This is a comma
     * delimited list of MIME types. If the list is empty, all
     * MIME types except for the blocked ones are allowed.
     */
    std::vector<std::string> allowed_mime_types;
    /*! Content type explicitly blocked on the sender id type.
     * This is a coma delimited list of content types on the
     * sender id.
     */
    std::vector<std::string> blocked_mime_types;
    /*! this is the id of the gateway / connector to the downstream
     * system that will handle the message delivery. The known
     * gateways/connectors and the respective connection parameters
     * for the gateway are configured separately.
     */
    std::string gateway_id;
    /// The date the resource was created
    std::int64_t last_update_date;
    std::vector<std::string> credential_parameter_list;


    /*! \class Resource SenderIdType.h scg_api/SenderIdType.h
     *
     * Resource class for SenderIdType
     *
     */
    class Resource : public ResourceImpl<SenderIdType>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<SenderIdType>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/sender_id_types")
        {
        }

        /*! List SenderIdTypes
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *          - id
         *          - name
         *          - capabilities
         *          - allowed_mime_types
         *          - blocked_mime_types
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

    };

    /*! \internal
     *
     * Not used for this class
     */
    void SetResource(Resource *res) noexcept { }

};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::SenderIdType,
    (std::string, id)
    (std::string, name)
    (std::string, description)
    (std::vector<std::string>, capabilities)
    (std::vector<std::string>, allowed_mime_types)
    (std::vector<std::string>, blocked_mime_types)
    (std::string, gateway_id)
    (std::int64_t, last_update_date)
    (std::vector<std::string>, credential_parameter_list))


