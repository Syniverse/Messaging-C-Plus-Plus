 #pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class Bridge Bridge.h scg_api/Bridge.h
 *
 */
class Bridge : public BaseData
{
public:
    std::string id;
    std::string external_id;
    std::int64_t completed_time = 0;
    std::int64_t activated_time = 0;
    std::int64_t created_date = 0;
    std::int64_t last_updated_date = 0;
    int version_number = 0;
    std::string state;
    bool bridge_audio = false;
    std::vector<std::string> call_ids;


    /*! \class Resource Bridge.h scg_api/Bridge.h
     *
     * Resource class for Bridge
     *
     */
    class Resource : public ResourceImpl<Bridge>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<Bridge>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/calling/bridges")
        {
        }

        /*! std::vector Bridges
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

        /*! Create an instance of a Bridge on the server.
         *
         * \arg obj Bridge object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const Bridge& br) {
            return Create_(br);
        }

        auto Create() {
            const Bridge br;
            return Create_(br);
        }

        /*! Update a Bridge on the server
         *
         * The Bridge instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const Bridge& br) {
            return Update_(br);
        }

        /*! Delete a Bridge
         *
         * \arg id of the Bridges to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a Bridge object from the server.
         *
         * \arg id of the Bridge you want.
         *
         * \return unique pointer to a Bridge
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "external_id", "completed_time",
                "activated_time", "created_date"
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

    /*! Update a Bridge on the server
     *
     * The Bridge instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a Bridge on the server
     *
     * The Bridge instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a Bridge if you have the ID using
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
    scg_api::Bridge,
    (std::string, id)
    (std::string, external_id)
    (std::int64_t, completed_time)
    (std::int64_t, activated_time)
    (std::int64_t, created_date)
    (std::int64_t, last_updated_date)
    (int, version_number)
    (std::string, state)
    (bool, bridge_audio)
    (std::vector<std::string>, call_ids))

