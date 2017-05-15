#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class Channel Channel.h scg_api/Channel.h
 *
 * Channels represent a group of sender addresses. Channels are
 * created to create a mechanism to deliver messages over different
 * media (if sender addresses of different type are used in a
 * channel) or to increase throughput (by adding multiple sender
 * ids to a single channel).
 */
class Channel : public BaseData
{
public:
    /// [read only] The channel identifier, allocated by SCG when channel is created
    std::string id;
    /// [read only]  SHARED/PRIVATE/PUBLIC
    std::string ownership;
    /// [read only] The date the resource was created.
    std::int64_t created_date = 0;
    /// [read only] The data the resource was last changed.
    std::int64_t last_update_date = 0;
    /// [read only] Application id
    std::int64_t application_id = 0;

    // Special
    int version_number = 0;

    // Read / Write
    /// User Specified Name of the Channel
    std::string name;
    /*! Determines the chgannel priority- high, normal or low. SMG
     * will deliver to target system first for messages in high
     * priority channels, then normal and at the end low priority.
     * Message size does not affect the message priority.
     */
    std::string priority;

    /// Description of the Channel
    std::string description;

    std::vector<std::string> message_templates;

    /*! \class Resource Channel.h scg_api/Channel.h
     *
     * Resource class for Channel
     *
     */
    class Resource : public ResourceImpl<Channel>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<Channel>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/channels")
        {
        }

        /*! List Channels
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *        - id
         *        - name
         *        - priority
         *        - ownership
         *        - created_date
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a Channel on the server.
         *
         * \arg obj Channel object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const Channel& ch) {
            return Create_(ch);
        }

        /*! Update a Channel on the server
         *
         * The Channel instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const Channel& ch) {
            return Update_(ch);
        }

        /*! Delete a Channel
         *
         * \arg id of the Channels to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a Channel object from the server.
         *
         * \arg id of the Channel you want.
         *
         * \return unique pointer to a Channel
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "ownership", "created_date", "last_update_date",
                "application_id"
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

    /*! Update a Channel on the server
     *
     * The Channel instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a Channel on the server
     *
     * The Channel instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a Channel if you have the ID using
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
    scg_api::Channel,
    (std::string, id)
    (std::string, ownership)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (std::int64_t, application_id)
    (int, version_number)
    (std::string, name)
    (std::string, priority)
    (std::string, description)
    (std::vector<std::string>, message_templates))

