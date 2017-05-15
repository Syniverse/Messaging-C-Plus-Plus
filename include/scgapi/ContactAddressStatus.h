
#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class ContactAddressStatus ContactAddressStatus.h scg_api/ContactAddressStatus.h
 *
 * The contact address status resource is used to keep the current
 * consent status of the contact address. The contact address status
 * is customer specific i.e. the same address may be tracked in
 * SCG separately for each customer.
 */
class ContactAddressStatus : public BaseData
{
public:
    // Read Only
    /*! [read only] Unique identifier, allocated by SCG at the time
     * of address creation. This is a combination of an address type/value
     * and sender id.
     */
    std::string id;
    /// [read only] The creation date of the object.
    std::int64_t created_date = 0;
    /// [read only] The last modification date of the object.
    std::int64_t last_update_date = 0;

    // Special
    int version_number = 0;

    // Read/Write
    /// The address type- MDN (for phone numbers), EMAIL, SOCIAL, PUSH (…)
    std::string address_type;
    /// The actual value of the address. For SMS/MMS this will be the MDN.
    std::string address;
    ///  the sender id accociated with the address type/value
    std::string sender_id;
    /*!  this is the current optin status for the specific address /
     * sender id. The possible values are:
     *   - NONE (no optin or optout message received from the address
     *          on the specific sender id)
     *   - OPTIN (user has explicitly opted in to the sender id)
     *   - OPTOUT (user has explicitly opted out of the sender id)
     *   - BLACKLIST (blacklisted address - will be treated as
     *          explicit OPTOUT)
     *   - WHITELIST (whitelisted address - will be treated as
     *          explicit OPTIN)
     *
     * \NOTE Whitelisted or blacklisted status can not be overwritten
     *          by the end-user by sending messages to the sender id.
     *          Such users can be only un-listed by changing the
     *          optin-status explicitly put POST to address status ID
     */
    std::string consent_status;
    /// The application id this contact belong to
    std::int64_t application_id = 0;

    /*! \class Resource ContactAddressStatus.h scg_api/ContactAddressStatus.h
     *
     * Resource class for ContactAddressStatus
     *
     */
    class Resource : public ResourceImpl<ContactAddressStatus>
    {
    public:
        Resource(Session& session)
        : ResourceImpl(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/consent/contact_address_statuses")
        {
        }

        /*! List ContactAddressStatus
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *        - id
         *        - address_type
         *        - address
         *        - sender_id
         *        - consent_status
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a ContactAddressStatus on the server.
         *
         * \arg obj ContactAddressStatus object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const ContactAddressStatus& obj) {
            return Create_(obj);
        }

        /*! Delete a ContactAddressStatus
         *
         * \arg id of the ContactAddressStatuss to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a ContactAddressStatus object from the server.
         *
         * \arg id of the ContactAddressStatus you want.
         *
         * \return unique pointer to a ContactAddressStatus
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        void UpdateConsent(const ContactAddressStatus& obj,
                           const std::string& consent) {

            ContactAddressStatus tmp;
            tmp.id = obj.id;
            tmp.consent_status = consent;
            tmp.version_number = obj.version_number;

            Update_(tmp);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "created_date", "last_update_date", "application_id"
            };

            return &names;
        }
    };

    /*! Assign a data object to a resource.
     *
     * Can be used to distribute data objects among a pool of
     * resources, each using their own Scg instances and worker-threads.
     */
    void SetResource(Resource *res) {
        res_ = res;
        res_exists_ = res_->GetExists();
    }

    /*! Delete a ContactAddressStatus on the server
     *
     * The ContactAddressStatus instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a ContactAddressStatus if you have the ID using
     * the Resource::Delete method.
     */
    void Delete() {
        VerifyForOperations();
        res_->Delete(id);
    }

    /*! Change the consent and updates it at the server side.
     *
     * The ContactAddressStatus instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     */
    void SetConsent(const std::string& consent) {
        VerifyForOperations();
        res_->UpdateConsent(*this, consent);
        consent_status = consent;
    }

private:
    void VerifyForOperations() {
        VerifyForOperations_(id);
    }

    Resource *res_ = {};
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::ContactAddressStatus,
    (std::string, id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (int, version_number)
    (std::string, address_type)
    (std::string, address)
    (std::string, sender_id)
    (std::string, consent_status)
    (std::int64_t, application_id))

