#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class ContactAddressHistory ContactAddressHistory.h scg_api/ContactAddressHistory.h
 *
 * The contact address status history resource is used to track the
 * received keywords from a given contact address and the changes
 * in the consent status of the contact address. The contact address
 * status is customer specific i.e. the same address may be tracked
 * in SCG separately for each customer.
 */
class ContactAddressHistory : public BaseData
{
public:
    /*! Unique identifier, allocated by SCG at the time of the history
     * record creation.
     */
    std::string id;
    /// The value of the contact's address. For SMS/MMS this will be the MDN.
    std::string msisdn;
    /// The sender id that MO/MT message was sent to/from
    std::string sender_id;
    /*! The source of the contact's consent status information:
     *   - MESSAGE - when the consent status has changed as a result of
     *          a MO message sent by the contact (SCG managed consent)
     *   - CARRIER  - when a blacklist file was provided by a
     *          carrier and loaded in
     *   - COMPANY - when the user of SCG has supplied the
     *          information (USER managed consent)
     */
    std::string source;
    /// The new consent status of the contact
    std::string status;
    /*! Timestamp indicating when the MO message that triggered the
     * consent status change was received.
     *
     * \todo Check data-type
     */
    std::string timestamp;
    /*! The body of the actual MO message that triggered the consent
     * status change.
     */
    std::string message;
    /// The keyword detected by the application
    std::string keyword;
    /// The application id this contact belon to
    std::int64_t application_id = 0;
    /// The creation date of the object.
    std::int64_t created_date;
    /// The last modification date of the object.
    std::int64_t last_update_date;

    int version_number = 0;

    /*! \class Resource ContactAddressHistory.h scg_api/ContactAddressHistory.h
     *
     * Resource class for ContactAddressHistory
     *
     */
    class Resource : public ResourceImpl<ContactAddressHistory>
    {
    public:
        Resource(Session& session)
        : ResourceImpl(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/consent/contact_address_history")
        {
        }

        /*! List ContactAddressHistorys
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const ListParameters *lp = nullptr) {
            return List_(nullptr, lp);
        }
    };

    /*! \internal
     *
     * Not used for this class
     */
    void SetResource(Resource * /*res*/) noexcept { }
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::ContactAddressHistory,
    (std::string, id)
    (std::string, msisdn)
    (std::string, sender_id)
    (std::string, source)
    (std::string, status)
    (std::string, timestamp)
    (std::string, message)
    (std::string, keyword)
    (std::int64_t, application_id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (int, version_number))

