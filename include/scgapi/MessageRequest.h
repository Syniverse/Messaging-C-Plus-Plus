#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"
#include "scgapi/Message.h"

namespace scg_api {

/*! \class MessageRequest MessageRequest.h scg_api/MessageRequest.h
 *
 * Message Requests represent instances of messages which have been
 * sent by Applications to Contacts.
 */
class MessageRequest : public BaseData
{
public:
    // Read only
    /// The identifier
    std::string id;
    /*! The number of contacts satisfying the criteria (or in the
     * contact group) who have an address on the channel the bulk
     * message is targeted for.
     */
    std::int64_t recipient_count = 0;
    /*! The number of messages successfully sent via API to the
     * underlying channel. The counts are increased continuously as
     * additional messages satisfy criteria.
     */
    std::int64_t sent_count = 0;
    /*! The number of messages sent for which some form of delivery
     * receipt confirmation has been received. The number of messages
     * successfully sent via API to the underlying channel. The counts
     * are increased continuously as additional messages satisfy criteria.
     */
    std::int64_t delivered_count = 0;
    /*!  The number of messages sent for which some form of delivery
     * receipt confirmation has been received. The number of messages
     * successfully sent via API to the underlying channel. The
     * counts are increased continuously as additional messages
     * satisfy criteria.
     */
    std::int64_t read_count = 0;
    /*! The number of messages where conversion has been confirmed.
     * This will apply for messages with URL where the application
     * requested a link track. The number of messages successfully
     * sent via API to the underlying channel. The counts are
     * increased continuously as additional messages satisfy criteria.
     */
    std::int64_t converted_count = 0;
    /*! The number of messages that have been created but not sent
     * because the message request was canceled.
     */
    std::int64_t canceled_count = 0;
    /// The number of messages that have expired or have failed delivery
    std::int64_t failed_count = 0;
    /*! For a MT message (one which was sent via this API), this
     * indicates the time when the API was called to request message
     * delivery. For a MO message, this is the point in time when the
     * SCG was notified of the inbound message from the operator
     * network/mediate channel.
     */
    std::int64_t created_date = 0;
    /// Timestamp of the last event associated with the message
    std::int64_t last_update_date = 0;

    // Read/Write
    /*! The channel or Sender Id over/from which the message is to be sent.
     *
     * - To send from a sender ID, the ID should be prefixed with 'sender_id'.
     * - To send from a channel the ID should be prefixed with 'channel-'.
     */
    std::string from;
    /// The identifier of the conversation for conversation threading.
    std::string conversation_id;
    /*! For MT messages this will be a list of one or more recipient
     * addresses, contact ids, contact group ids.
     */
    std::vector<std::string> to;
    /// The customer provided campaign id.
    std::string campaign_id;
    std::string program_id;
    /// The subject – a short summary of the message’s purpose.
    std::string subject;
    /// ID of the application that has created the message request
    std::string application_id;
    /*! Application provided unique tracking ID for the message. Can
     * be used as an alternative key in query methods.
     */
    std::string external_id;
    /*! The status of the message request
     *
     * - SUBMITTED
     * - ACCEPTED
     * - REJECTED
     * - PREPARING (while in the preparing state, the recipient cont
     *          may increase with time i.e. SCG will keep track of
     *          the preparing step to allow polling on the resource
     *          to have user see progress)
     * - TRANSMITTING (while in the transmission state, the sent count
     *          will be updates as the messages are being sent)
     * - COMPLETED
     * - PAUSED (SCG will go into this state after preparing if pause flag is set)
     * - CANCELED Canceled (cancel can be requested at any time)
     *
     * \NOTE to resume paused messages, the status should be updated
     * by customer from PAUSED to TRANSMITTING
     */
    std::string state;
    /// A list of attachment IDs which are associated with this message.
    std::vector<std::string> attachments;
    /*! This is the original message body as supplied by application,
     * prior to keyword processing.
     *
     * It can reference a message template and supply the variables
     * assignments for use with a template or contain the actual
     * message body.
     */
    std::string body;
    /*! Field indicating if the message delivery is dependent on the
     * double optin for the recipient. Any contacts/destination MDNs
     * that should receive this message but do not have double optin
     * in associated with the sender id would not be delivered
     * (message will be created but will be in 'failed' status and
     * will not be sent to the recipient). Values are:
     *
     * - OPT_IN (message will be sent only if user explicitly opted in)
     * - OPT_OUT (message will be sent unless user explicitly opted out)
     * - NONE (message will be sent irrespectively of optin status)
     */
    std::string consent_requirement;
    /// The criteria string which was used in the bulk transmission, if any.
    std::string criteria;
    /// The date and time that this message should be delivered.
    std::string scheduled_delivery_time;
    /*! Determines what time zone to apply to the scheduled delivery time:
     *
     * - UTC- means that the scheduled delivery time as supplied in the
     *          request is expressed in UTC time
     * - LOCAL- means that the scheduled delivery time as supplied in
     *          the request is expressed in LOCAL recipient time.
     *          The time zone of the recipient is determined by SCG
     *          automatically based on-
     *            (1) actual location of the user (if LBS active and location known)
     *            (2) zip code associated with the user
     *            (3) MDN of the user (based on country)
     */
    std::string scheduled_delivery_time_zone;
    /*! This is the timestamp after which the messages resulting from
     * the message request should no longer be sent.
     */
    std::string expiry_time;
    /*! If this flag is set to true then this will be processed and
     * Out bound messages will be created but the messages will not
     * be delivered to the clients.
     */
    bool test_message_flag = false;
    /*! This is a flag that tells SCG to generate the messages but
     * not to deliver them.
     */
    bool pause_before_transmit = false;
    /*! This is the timestamp after which all paused messages will
     * expire and will be removed from the outbound queue. The
     * maximum expiry time is 24h. If application creates a message
     * request with longer pause expiry time, the message request
     * will fail.
     */
    std::string pause_expiry_time;

    std::vector<std::string> contact_delivery_address_priority;
    std::string failover;
    double price_threshold;
    std::vector<std::string> sender_id_sort_criteria;
    std::string src_language;
    std::string dst_language;
    bool translate = false;
    int translations_count = 0;
    int translations_failed_count = 0;
    int translations_performed_count = 0;


    /*! \class Resource MessageRequest.h scg_api/MessageRequest.h
     *
     * Resource class for MessageRequest
     *
     */
    class Resource : public ResourceImpl<MessageRequest>
    {
    public:
        struct StateRequest {
            std::string state;
        };

        Resource(Session& session)
        : ResourceImpl<MessageRequest>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/message_requests")
        {
        }

        /*! List MessageRequests
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *          - id
         *          - from
         *          - conversation_id
         *          - to
         *          - campaign_id
         *          - program_id
         *          - subject
         *          - application_id
         *          - external_id
         *          - state
         *          - scheduled_delivery_time
         *          - expiry_time
         *          - test_message_flag
         *          - pause_before_transmit
         *          - pause_expiry_time
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a MessageRequest on the server.
         *
         * \arg obj MessageRequest object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const MessageRequest& obj) {
            return Create_(obj);
        }

        /*! Update a MessageRequest on the server
         *
         * The MessageRequest instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const MessageRequest& obj) {
            return Update_(obj);
        }

        /*! Delete a MessageRequest
         *
         * \arg id of the MessageRequests to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a MessageRequest object from the server.
         *
         * \arg id of the MessageRequest you want.
         *
         * \return unique pointer to a MessageRequest
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! Set the state of a MessageRequest on the server
         *
         * \arg id ID of the MessageRequest
         * \arg state A new, vlid state to set.
         */
        auto SetState(const std::string& id, const std::string& state) {
            StateRequest payload = { state };

            auto url = GetResourceUrl() + "/" + id;
            DoPost<decltype(payload)>(payload, url);
        }

        /*! \internal */
        std::string GetMessagesUrl(const std::string& id) const {
            return GetResourceUrl() + "/" + id + "/messages";
        }

        /*! Get the list of messages associated with the provided
         * Message Request ID.
         *
         * The Filters are the same as for Message::Resource::List()
         *
         * \returns  Iterator to the result-set.
         *
         */
        auto ListMessages(const std::string& id,
                          const filter_t *filter = nullptr,
                          const ListParameters *lp = nullptr) {
            if (!message_res_) {
                message_res_ = std::make_unique<Message::Resource>(
                    GetSession(), GetMessagesUrl(id));
            }

            return message_res_->List(filter, lp);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "recipient_count",
                "sent_count", "delivered_count",
                "read_count", "converted_count",
                "canceled_count", "failed_count",
                "created_date", "last_update_date"
            };

            return &names;
        }

    private:
        std::unique_ptr<Message::Resource> message_res_;
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

    /*! Delete a MessageRequest on the server
     *
     * The MessageRequest instance must be received
     * from the server. You cannot delete an instance
     * you have instantiated yourself.
     *
     * You can delete a MessageRequest if you have the ID using
     * the Resource::Delete method.
     */
    void Delete() {
        VerifyForOperations();
        res_->Delete(id);
    }

    /*! Change the state on the server to TRANSMITTING */
    void Resume() {
        VerifyForOperations();
        res_->SetState(id, "TRANSMITTING");
    }

    /*! Cancel pending processing */
    void Cancel() {
        VerifyForOperations();
        res_->SetState(id, "CANCELED");
    }

    /*! Get the list of messages associated with this MessageRequest
     *
     * The Filters are the same as for Message::Resource::List()
     *
     * \returns Iterator to the result-set.
     *
     */
    auto ListMessages(const filter_t *filter = nullptr,
        const ListParameters *lp = nullptr) {
        VerifyForOperations();
        return res_->ListMessages(id, filter);
    }


private:
    void VerifyForOperations() {
        VerifyForOperations_(id);
    }

    Resource *res_ = {};
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::MessageRequest,
    (std::string, id)
    (std::int64_t, recipient_count)
    (std::int64_t, sent_count)
    (std::int64_t, delivered_count)
    (std::int64_t, read_count)
    (std::int64_t, converted_count)
    (std::int64_t, canceled_count)
    (std::int64_t, failed_count)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (std::string, from)
    (std::string, conversation_id)
    (std::vector<std::string>, to)
    (std::string, campaign_id)
    (std::string, program_id)
    (std::string, subject)
    (std::string, application_id)
    (std::string, external_id)
    (std::string, state)
    (std::vector<std::string>, attachments)
    (std::string, body)
    (std::string, consent_requirement)
    (std::string, criteria)
    (std::string, scheduled_delivery_time)
    (std::string, scheduled_delivery_time_zone)
    (std::string, expiry_time)
    (bool, test_message_flag)
    (bool, pause_before_transmit)
    (std::string, pause_expiry_time)
    (std::vector<std::string>, contact_delivery_address_priority)
    (std::string, failover)
    (double, price_threshold)
    (std::vector<std::string>, sender_id_sort_criteria)
    (std::string, src_language)
    (std::string, dst_language)
    (bool, translate)
    (int, translations_count)
    (int, translations_failed_count)
    (int, translations_performed_count))


BOOST_FUSION_ADAPT_STRUCT(
    scg_api::MessageRequest::Resource::StateRequest,
    (std::string, state))
