
#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class Keyword Keyword.h scg_api/Keyword.h
 *
 * Keywords are defined by customer and represent special words that
 * customers can send to a number belonging to a customer. Keywords
 * are detected by SCG and added to the customer profile. Certain
 * actions can be associated with a keyword received from a customer.
 */
class Keywords : public BaseData
{
public:
    /// [read only] unique keyword id
    std::string id;
    /// [read only] The last modification date of the object.
    std::int64_t last_update_date = 0;
    /// The application id this contact belong to
    std::int64_t application_id = 0;
    /// [read only] The creation date of the object.
    std::int64_t created_date = 0;

    // Special
    int version_number = 0;

    // Read/Write
    /// name of the keyword
    std::string name;
    /// description of the keyword
    std::string description;
    /*!  the keyword. For example 'SPORTS'. The keyword can be expressed
     * in form of a regular expression. POSIX Extended syntax is supported.
     */
    std::string value;
    /*! SENSITIVE/INSENSITIVE specifies whether the keyword is case
     * sensitive or not. default is not case sensitive.
     */
    std::string case_value;
    /*! The sender id on which the keyword is valid. When this is
     * empty, any sender id associated with the given customer will
     * be sensitive to this keyword.
     */
    std::string sender_id;
    /*! the date range for which the keyword is valid
     * \todo Check data type
     */
    std::string valid_from;
    /// the date range for which the keyword is valid
    std::string valid_to;
    /*! additional information, value is at customer discretion, this
     * attribute is added to the keyword event whenever the keyword
     * is detected
     */
    std::string associated_info;
    /// The campaign id
    std::string campaign_id;
    /*! the type of the keyword:
     *   - RESERVED those are the platform specific keywords that can
     *          not be used by customer in any campaign. Those
     *          keywords will always exist, the company will never
     *          need to add them.
     *   - COMPANY- company specific keywords
     */
    std::string type;
    /*! Actions to be applied when the keyword has been received.
     * Multiple actions can be specified.
     *
     * - OPTOUT -> mark the user as explicitly opted out (and forward
     *          message to Consent Manager)
     * - OPTIN > mark the user as explicitly opted in (and forward
     *          message to Consent Manager)
     * - STORE -> no action, just record in user profile
     */
    std::vector<std::string> actions;
    /*! If specified then SMG will send a predefined message back to
     * the user. This attribute will contain the name of the message
     * template to be used in the automatic reply.
     */
    std::string reply_template;

    /*! \class Resource Keyword.h scg_api/Keyword.h
     *
     * Resource class for Keyword
     *
     */
    class Resource : public ResourceImpl<Keywords>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<Keywords>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/keywords")
        {
        }

        /*! List Keywords
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *        - id
         *        - value
         *        - sender_id
         *        - campaign_id
         *        - type
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a Keyword on the server.
         *
         * \arg obj Keyword object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const Keywords& obj) {
            return Create_(obj);
        }

        /*! Update a Keyword on the server
         *
         * The Keyword instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const Keywords& obj) {
            return Update_(obj);
        }

        /*! Delete a Keyword
         *
         * \arg id of the Keywords to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a Keyword object from the server.
         *
         * \arg id of the Keyword you want.
         *
         * \return unique pointer to a Keyword
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "created_date", "last_update_date",
                "application_id"
            };

            return &names;
        }

        /*! \internal */
        const restc_cpp::JsonFieldMapping *
        GetJsonFieldMapping() const noexcept override {
            static const restc_cpp::JsonFieldMapping mapping = {{{"case_value", "case"}}};
            return &mapping;
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

    /*! Update a Keyword on the server
     *
     * The Keyword instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a Keyword on the server
     *
     * The Keyword instance must be received
     * from the server. You cannot delete an instance
     * you have instantiated yourself.
     *
     * You can delete a Keyword if you have the ID using
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
    scg_api::Keywords,
    (std::string, id)
    (std::int64_t, last_update_date)
    (std::int64_t, application_id)
    (std::int64_t, created_date)
    (int, version_number)
    (std::string, name)
    (std::string, description)
    (std::string, value)
    (std::string, case_value)
    (std::string, sender_id)
    (std::string, valid_from)
    (std::string, valid_to)
    (std::string, associated_info)
    (std::string, campaign_id)
    (std::string, type)
    (std::vector<std::string>, actions)
    (std::string, reply_template))


