 #pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class Message Message.h scg_api/Message.h
 *
 * A Message resource is created for every MO or MT message that is
 * processed by SCG. For MT messages, there is an Message created for
 * every recipient of the corresponding message request. For MO
 * messages there is one message created. This resource is read only
 * to an application.
 */
class Message : public BaseData
{
public:
    struct MessageFragmentInfo {
        std::string fragment_id;
        std::string fragment_state;
        double charge = {};
        int failure_code = {};
        std::string failure_details;
        std::string protocol_error;
        std::string external_id;
        std::string delivery_report_reference;
    };


    /// Unique identifier
    std::string id;

    /// The unique id of the associated message request resource.
    std::string message_request_id;
    /*! This is list of id-s (one for every fragment) issued for this
     * message by the downstream messaging system.
     */
    std::string external_transaction_ids;
    std::string external_message_request_id;
    /*! ID of the application that has created the message. May not
     * be present on incoming messages when the received message
     * could not be associated with a previous MT message
     */
    std::string application_id;
    /*! Application provided unique tracking ID for the message.
     * Can be used as an alternative key in query methods.
     */
    std::string application_tracking_id;
    /// The identifier of the conversation for conversation threading.
    std::string conversation_id;
    /*! The campaign id- A unique identifier for a particular
     * messaging campaign or program
     */
    std::string campaign_id;
    /// Message direction- MO for received messages and MT sent messages
    std::string direction;
    /*! The sender_id associated with the message delivery channel.
     * For MT messages this will be the sender_id used to deliver
     * the message. For MO messages this will be the sender_id
     * corresponding to the downstream system which generated the
     * message (determined by SMG based on the from/to address
     * accordingly)
     */
    std::string customer_sender_id;
    /// The sender address.
    std::string from_address;
    /// The recipient address.
    std::string to_address;
    /*! The status of the message to this recipient.
     * Valid values for MT messages are:
     *   - CREATED (message was created and is in the queue of SMG)
     *   - SENT (message was sent to the outbound delivery system)
     *   - DELIVERED (message was delivered to end-device of the user)
     *   - READ (message was read by the user)
     *   - CONVERTED (message was converted i.e. end user took action upon message)
     *   - FAILED (message delivery failed)
     *   - EXPIRED (message was too old to be sent)
     *   - SCHEDULED (message is created but not submitted for sending)
     *   - TEST (message was created but the request was a test request)
     *   - PAUSED (message was created but the request was to pause before delivery)
     *   - DELETED
     *
     * SMG will automatically set the status of the message to
     * converted when the message contains a 'tracked' URL and the
     * link was clicked by the recipient.
     *
     * Alternatively the customer also externally can track conversion
     * and could update the message status put applying the PUT method
     * and specifying status = CONVERTED and the conversion timestamp
     *
     * Valid values for MO Messages are:
     *   - RECEIVED (received at SMG)
     *   - PROCESSED (when the customer has accessed the message and updated the status)
     *   - DELETED (when the message was deleted by the customer)
     */
    std::string state;
    ///  INVALID_RECIPIENT, NO_CONSENT, OTHER
    std::string failure_code;
    /// text description that gives more detail on failed messages
    std::string failure_details;
    /// Subject of the message.
    std::string subject;
    /// The message body as sent to the recipient.
    std::string body;
    /// The timestamp when the message was submitted to downstream system for delivery
    std::int64_t sent_date = 0;
    /// The timestamp when the message has been delivered, corresponds to DR date.
    std::int64_t delivered_date = 0;
    /*!  The timestamp when the message was converted. This will be
     * set automatically by SMG for messages with link tracking.
     * For external tracking, the customer must provide the converted
     * timestamp
     */
    std::int64_t converted_date = 0;
    /*! Supplies information where the conversion information comes
     * from. SMG/URL means the message was marked as converted by the
     * link tracking service. For customer converted messages, the
     * content of this attribute is determined by the customer and
     * must be provided on the PUT request which marks the message as
     * 'converted'.
     */
    std::string conversion_info_source;
    /*! The Message ID of a previously delivered message which the
     * current message is a reply to.
     */
    std::string reply_to;
    /// A list of attachment IDs which are associated with this message.
    std::vector<std::string> attachments;
    /*! The date and time that this message should be delivered. This
     * is the user specific time as calculated by the SCG based on
     * the destination time zone and requested time zone calculation.
     */
    std::string type;
    std::string message_delivery_provider;
    std::string contact_id;
    // TODO: Replace double with a type better suited to represent money
    double price = {};
    std::string language;
    std::string failed_translation;
    std::string failed_origin_id;
    std::string failover;
    std::string scheduled_delivery_time;
    /*! This is the timestamp after which the messages will no longer
     * be sent to the downstream system for delivery
     */
    std::string expiry_time;
    /*! The timestamp when the message was created, which corresponds
     * to the time when the bulk message POST was done by the client.
     */
    std::int64_t created_date = 0;
    /// The data the resource was last changed
    std::string last_update_date;
    /// Status for each fragment for this message
    std::vector<MessageFragmentInfo> fragments_info;

    /*! Consent requirement.
     *
     * One of "NONE", "OPT_IN" or "OPT_OUT" if defined.
     */
    std::string consent_requirement;


    /*! \class Resource Message.h scg_api/Message.h
     *
     * Resource class for Message
     *
     */
    class Resource : public ResourceImpl<Message>
    {
    public:
        struct StateRequest {
            std::string state;
        };

        Resource(Session& session)
        : ResourceImpl<Message>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/messages")
        {
        }

        Resource(Session& session, const std::string& url)
        : ResourceImpl<Message>(session, url)
        {
        }

        /*! List Messages
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *        - id
         *        - message_request_id
         *        - external_transaction_ids
         *        - application_id
         *        - application_tracking_id
         *        - conversation_id
         *        - campaign_id
         *        - direction
         *        - customer_sender_id
         *        - from_address
         *        - to_address
         *        - state
         *        - failure_code
         *        - subject
         *        - body
         *        - sent_date
         *        - delivered_date
         *        - converted_date
         *        - reply_to
         *        - scheduled_delivery_time
         *        - expiry_time
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Delete a Message
         *
         * \arg id of the Messages to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

         /*! Get a Message object from the server.
         *
         * \arg id of the Message you want.
         *
         * \return unique pointer to a Message
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! Set the satate of a Message on the server.
         *
         * \arg id Unique identifier of the message
         * \arg state The new state.
         */
        auto SetState(const std::string& id, const std::string& state) {
            StateRequest payload = { state };

            auto url = GetResourceUrl() + "/" + id;
            DoPost<decltype(payload)>(payload, url);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {"id"};
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

    /*! Delete a Message on the server
     *
     * The Message instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a Message if you have the ID using
     * the Resource::Delete method.
     */
    void Delete() {
        VerifyForOperations();
        res_->Delete(id);
    }

    // Set the state of the message on the server to PROCESSED
    void SetStateProcessed() {
        VerifyForOperations();
        res_->SetState(id, "PROCESSED");
    }

    // Set the state of the message on the server to CONVERTED
    void SetStateConverted() {
        VerifyForOperations();
        res_->SetState(id, "CONVERTED");
    }

private:
    void VerifyForOperations() {
        VerifyForOperations_(id);
    }

    Resource *res_ = {};
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Message::MessageFragmentInfo,
    (std::string, fragment_id)
    (std::string, fragment_state)
    (double, charge)
    (int, failure_code)
    (std::string, failure_details)
    (std::string, protocol_error)
    (std::string, external_id)
    (std::string, delivery_report_reference))


BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Message,
    (std::string, id)
    (std::string, message_request_id)
    (std::string, external_transaction_ids)
    (std::string, external_message_request_id)
    (std::string, application_id)
    (std::string, application_tracking_id)
    (std::string, conversation_id)
    (std::string, campaign_id)
    (std::string, direction)
    (std::string, customer_sender_id)
    (std::string, from_address)
    (std::string, to_address)
    (std::string, state)
    (std::string, failure_code)
    (std::string, failure_details)
    (std::string, subject)
    (std::string, body)
    (std::int64_t, sent_date)
    (std::int64_t, delivered_date)
    (std::int64_t, converted_date)
    (std::string, conversion_info_source)
    (std::string, reply_to)
    (std::vector<std::string>, attachments)
    (std::string, type)
    (std::string, message_delivery_provider)
    (std::string, contact_id)
    (double, price)
    (std::string, language)
    (std::string, failed_translation)
    (std::string, failed_origin_id)
    (std::string, failover)
    (std::string, scheduled_delivery_time)
    (std::string, expiry_time)
    (std::int64_t, created_date)
    (std::string, last_update_date)
    (std::vector<scg_api::Message::MessageFragmentInfo>, fragments_info)
    (std::string, consent_requirement))


BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Message::Resource::StateRequest,
    (std::string, state))
