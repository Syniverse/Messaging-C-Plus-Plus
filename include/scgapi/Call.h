 #pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class Call Call.h scg_api/Call.h
 *
 */
class Call : public BaseData
{
public:
    std::string id;
    std::string external_id;
    std::int64_t start_time = 0;
    std::int64_t answer_time = 0;
    std::int64_t end_time = 0;
    int chargeable_duration = 0;
    std::string failure_code;
    std::string failure_details;
    std::int64_t created_date = 0;
    std::int64_t last_updated_date = 0;

    int version_number = 0;

    std::string from;
    std::string from_address;
    std::string to;
    int answer_timeout = 0;
    std::string state;
    std::string direction;
    std::string bridge_id;
    bool recording_enabled = false;


    /*! \class Resource Call.h scg_api/Call.h
     *
     * Resource class for Call
     *
     */
    class Resource : public ResourceImpl<Call>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<Call>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/calling/calls")
        {
        }

        /*! std::vector Calls
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *
         * \arg lp std::vector Parameters
         *
         * \returns Iterator to the result-set.
         */
         auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
         }

        /*! Create an instance of a Call on the server.
         *
         * \arg obj Call object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const Call& call) {
            return Create_(call);
        }

        /*! Update a Call on the server
         *
         * The Call instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const Call& call) {
            return Update_(call);
        }

        /*! Delete a Call
         *
         * \arg id of the Calls to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a Call object from the server.
         *
         * \arg id of the Call you want.
         *
         * \return unique pointer to a Call
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "external_id", "start_time", "answer_time",
                "end_time", "chargeable_duration", "failure_code",
                "failure_details", "created_date", "last_updated_date"
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

    /*! Update a Call on the server
     *
     * The Call instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a Call on the server
     *
     * The Call instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a Call if you have the ID using
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
    scg_api::Call,
    (std::string, id)
    (std::string, external_id)
    (std::int64_t, start_time)
    (std::int64_t, answer_time)
    (std::int64_t, end_time)
    (int, chargeable_duration)
    (std::string, failure_code)
    (std::string, failure_details)
    (std::int64_t, created_date)
    (std::int64_t, last_updated_date)
    (int, version_number)
    (std::string, from)
    (std::string, from_address)
    (std::string, to)
    (int, answer_timeout)
    (std::string, state)
    (std::string, direction)
    (std::string, bridge_id)
    (bool, recording_enabled))
