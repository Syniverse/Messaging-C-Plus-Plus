
#pragma once

#include <set>
#include <map>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class SenderId SenderId.h scg_api/SenderId.h
 *
 * A Sender Address is a specific address (SMS short code, long code,
 * email address, facebook account ID, etc.) that an Application can
 * send messages from or receive messages to. The Sender Address must
 * be provisioned with the SCG system before it can be used. Once a
 * Sender Address has been provisioned it can be added to a Channel
 * and messages can by sent using the Channel. Alternatively
 * messages can be sent from a sender id without a channel. Sender
 * addresses are customer specific - a customer will be able to
 * access only his own sender addresses
 *
 */

class SenderId : public BaseData
{
public:
    using billing_t = std::map<std::string, std::string>;

     // Read only
    /// [read only] unique id of the sender address
    std::string id;
    /// The application id this contact belong to
    std::int64_t application_id = 0;
    /// [read only] The date the resource was created
    std::int64_t created_date = 0;
    /// [read only] The data the resource was last changed
    std::int64_t last_update_date = 0;

    // Special
    /*! An ascending version number of the resource. 1 upon creation,
     * incremented by one for each modification done. Used to
     * implement optimistic locking
     */
    int version_number = 0;

    // Read / write
    /*! For shared short codes, this is the sender id of the
     * preprovisioned short code that belongs to Syniverse. Whenever
     * a customer wants to use a preprovisioned, shared code, the
     * parent sender address id will indicate the sender id that was
     * used to create customer version of the sender id.
     */
    std::string parent_id;
    /// name of the sender address
    std::string name;
    /*! this field determines the ownership category of a shortcode.
     * Applies only for SMS type of sender id's. Following values
     * are allowed:
     *  - PRIVATE: represents a short code that is private for use
     *           by one company only
     *  - SHARED: represents a short code that is owned by
     *          Syniverse and where multiple customers can use it.
     *          When a customer wants to use a SHARED sender id, a
     *          new PRIVATE sender address will be created with
     *          parent sender id set to the SHARED sender id.
     *  - PREPROVISIONED: represents an address that is owned by
     *          Syniverse and can be selected by a customer. Upon a
     *          customer selecting a preprovisioned sender address,
     *          new sender id document as a copy of the PREPROVISIONED
     *          document is created, with state PRIVATE and parent_id
     *          pointing the original PREPROVISIONED sender address.
     *          The PREPROVISIONED sender address changes ownership
     *          to PURCHASED. Once the PRIVATE sender address,
     *          pointing to the PURCHASED is deleted, the PURCHASED
     *          sender changes its ownership back to PREPROVISIONED.
     *  - PURCHASED: PREPROVISIONED sender address, currently
     *          occupied by a customer.
     */
    std::string ownership;
    /*! reference to the sender address class that determines the
     * priority and throughput of the configured sender id.
     */
    std::string class_id;
    /*! reference the sender address type that determines the data
     * types allowed on the sender id and identifies the gateway used
     * to deliver the messages.
     */
    std::string type_id;
    /*! represents the sender address status. Following values are
     *  - PENDING_IMPLEMENTATION
     *  - IMPLEMENTED
     *  - ACTIVE
     *  - INACTIVE
     *  - BROKEN
     *  - PENDING_DELETE allowed:
     *
     * For each sender address that require provisioning, when a
     * sender address is requested, the initial status will be
     * 'pending implementation'
     */
    std::string state;
    /*! The sender address. The address must correspond to the sender
     * address type. For a2p sms this will be the shortcode or
     * longcode, for emails this will be the email address etc …
     */
    std::string address;
    /*! The content of this field is specific the sender type /
     * gateway associated with this sender.
     */
    std::string content_type;
    /*! A list of template ids that apply to this sender address. */
    std::vector<std::string> message_templates;
    /*! The country that the sender address can be used to send
     * messages to. This field will contain ISO country code. The
     * value is managed by Syniverse and not editable by the customer.
     */
    std::string country;
    /*! A list of carriers who are confirmed to have provisioned the
     * sender address. This will be a coma separate list of SPIDs.
     * The values are managed by Syniverse and not editable by the
     * customer
     */
    std::string operators;
    /*! A String containing the user’s credentials which is formatted
     * based on the Sender Address type.
     */
    std::string credentials;
    /*! represents customer requested features of the sender id - if
     * set to YES, means customer expects the sender id to support
     * replies to an address.
     */
    std::string two_way_required;
    /*! represents customer requested features of the sender id - if
     * set to YES, means customer expects the sender id to be
     * preserved
     */
    std::string keep_sender_address;

    // TODO: Document
    std::vector<std::string> applied_charges;
    /*! represents customer requested features of the sender id - if
     * set to YES, means customer expects the sender id to support DR.
     */
    std::string dr_required;
    /// Possible values- USER, SCG. Default- USER.
    std::string consent_managed_by;
    /*! Message types allowed on the sender ids from of this type-
     *  SMS, MMS, EMAIL, etc.
     */
    std::vector<std::string> capabilities;
    /*! If set, sending via this sender ID is limited to recipients
     * present in the customer's white list.
     */
    bool check_whitelist = false;

    billing_t billing;

    /*! \class Resource SenderId.h scg_api/SenderId.h
     *
     * Resource class for SenderId
     *
     */
    class Resource : public ResourceImpl<SenderId>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<SenderId>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/sender_ids")
        {
        }

        /*! List SenderIds
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *          - id
         *          - parent_id
         *          - name
         *          - ownership
         *          - class_id
         *          - type_id
         *          - state
         *          - country
         *          - operators
         *          - two_way_required
         *          - keep_sender_address
         *          - dr_required
         *          - consent_managed_by
         *          - capabilities
         *          - check_whitelist
         *          - created_date
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a SenderId on the server.
         *
         * \arg obj SenderId object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const SenderId& obj) {
            return Create_(obj);
        }

        /*! Update a SenderId on the server
         *
         * The SenderId instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const SenderId& obj) {
            return Update_(obj);
        }

        /*! Delete a SenderId
         *
         * \arg id of the SenderIds to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a SenderId object from the server.
         *
         * \arg id of the SenderId you want.
         *
         * \return unique pointer to a SenderId
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! Creates a new Sender ID Resource, copy of specified
         * PREPROVISIONED Sender ID, which becomes PURCHASED until
         * the private copy is deleted
         *
         * \arg parentId The Sender Id resource associated with the
         *              provided ID\
         * \returns id of the purchased SenderId.
         */
        std::string Purchase(const std::string& parentId) {
            restc_cpp::Request::args_t query_args = {
                 {"parent_id", parentId}
            };
            auto url = GetResourceUrl() + "/purchase";
            GenericReply rval;
            restc_cpp::SerializeFromJson(rval, DoPostNoBody(url, query_args));
            return rval.id;
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "application_id", "created_date",
                "last_update_date"
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

    /*! Update a SenderId on the server
     *
     * The SenderId instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a SenderId on the server
     *
     * The SenderId instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a SenderId if you have the ID using
     * the Resource::Delete method.
     */
    void Delete() {
        VerifyForOperations();
        res_->Delete(id);
    }

private:
    void VerifyForOperations() {
        VerifyForOperations_(id);
    }

    Resource *res_ = {};
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::SenderId,
    (std::string, id)
    (std::int64_t, application_id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (int, version_number)
    (std::string, parent_id)
    (std::string, name)
    (std::string, ownership)
    (std::string, class_id)
    (std::string, type_id)
    (std::string, state)
    (std::string, address)
    (std::string, content_type)
    (std::vector<std::string>, message_templates)
    (std::string, country)
    (std::string, operators)
    (std::string, credentials)
    (std::string, two_way_required)
    (std::string, keep_sender_address)
    (std::vector<std::string>, applied_charges)
    (std::string, dr_required)
    (std::string, consent_managed_by)
    (std::vector<std::string>, capabilities)
    (bool, check_whitelist)
    (scg_api::SenderId::billing_t, billing))

