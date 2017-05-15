
#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class MessageTemplate MessageTemplate.h scg_api/MessageTemplate.h
 *
 * Message Templates can be created and associated with a sender id.
 * A message can then be sent with the corresponding name-value
 * pairs to fill in the template. A sender id can be configured to
 * enforce template use so that only messages that use a template
 * can be sent on that sender id To use a template, the message
 * body will contain a special content type with a list of name
 * value pairs as the content.
 */
class MessageTemplate : public BaseData
{
public:
    // Read Only
    /// [read only] The identifier
    std::string id;
    /// The application id this contact belong to
    std::int64_t application_id = 0;
    /// [read only] The date the resource was created
    std::int64_t created_date = 0;
    /// [read only] The data the resource was last changed
    std::int64_t last_update_date = 0;

    // Read/Write
    /// The template is used either for VALIDATOR or as an actual TEMPLATE.
    std::string designation;
    /// The name of the template.
    std::string name;
    /*! template pattern can be used to enforce compliancy of the
     * message body with the submitted program brief. The template
     * pattern allows also to use the templates to form the message
     * body by using the template pattern and replacing the keywords
     * in the pattern with the values of submitted keywords in the
     * message send from the sender id.
     */
    std::string pattern;

    /*! \class Resource MessageTemplate.h scg_api/MessageTemplate.h
     *
     * Resource class for MessageTemplate
     *
     */
    class Resource : public ResourceImpl<MessageTemplate>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<MessageTemplate>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/message_templates")
        {
        }

        /*! List MessageTemplates
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *      - id
         *      - designation
         *      - name
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a MessageTemplate on the server.
         *
         * \arg obj MessageTemplate object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const MessageTemplate& obj) {
            return Create_(obj);
        }

        /*! Update a MessageTemplate on the server
         *
         * The MessageTemplate instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const MessageTemplate& obj) {
            return Update_(obj);
        }

        /*! Delete a MessageTemplate
         *
         * \arg id of the MessageTemplates to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a MessageTemplate object from the server.
         *
         * \arg id of the MessageTemplate you want.
         *
         * \return unique pointer to a MessageTemplate
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
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

    /*! Update a MessageTemplate on the server
     *
     * The MessageTemplate instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a MessageTemplate on the server
     *
     * The MessageTemplate instance must be received
     * from the server. You cannot delete an instance
     * you have instantiated yourself.
     *
     * You can delete a MessageTemplate if you have the ID using
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
    scg_api::MessageTemplate,
    (std::string, id)
    (std::int64_t, application_id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (std::string, designation)
    (std::string, name)
    (std::string, pattern))

