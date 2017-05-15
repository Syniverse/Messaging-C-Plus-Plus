#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class SenderIdClass SenderIdClass.h scg_api/SenderIdClass.h
 *
 * This is an immutable resource that represents the list of sender
 * classes that an be associated with a sender id.
 *
 * The sender class determines the type of messages
 * (notifications, alerts, commercial ….) as well as message
 * throughput and sending window restriction
 *
 * \note Instances of this class is read-only, as they cannot be
 * created or manipulated or changed (on the server) from
 * this API.
 */
class SenderIdClass : public BaseData
{
public:
    /// Unique id of the sender class
    std::string id;
    /// Name of the sender class
    std::string name;
    /// human readable description of the sender class
    std::string description;
    /// Primary channel designation: commercial, alerts, notifications …
    std::string designation;
    /*! list of countries to which the sender class applies, coma
     * delimited using iso country code. Allow a keyword of 'OTHER'
     * for sender classes that are not country specific.
     */
    std::string applicable_countries;
    /*! Maximum outbound channel throughput for a given customer per
     * destination country expressed in transactions per second
     */
    std::int64_t country_peak_throughput = 0;
    /*! Maximum outbound channel throughput for all customers using
     * the short or long code per destination country expressed in
     * transactions per second (this is the physical limit of the
     * sender address)
     */
    std::int64_t country_peak_total_throughput = 0;
    /*! Maximum outbound channel throughput for a given customer per
     * destination country expressed in transactions per day
     */
    std::int64_t country_daily_throughput = 0;
    /*! Allowed delivery time for the messages. Applying the time
     * validation, the time is to be considered in the time zone of
     * the recipient. This is a JSON string with a list of allowed
     * times by destination country. Countries are represented by an
     * ISO country code. A special country code of 'DEFAULT' is to
     * be used for the default country. For each country a delivery
     * window can be specified. Each weekday can have different
     * delivery window times. As weekday names, use of
     * following days- MON, TUE, WED, THU, FRI, SAT, SUN as well as
     * two additional keywords- HOL (representing local holiday in
     * the country) and OTH (representing all other days of week)
     */
    std::string delivery_window;
    /// The date the resource was created
    std::int64_t created_date = 0;
    /// The date the resource was last changed
    std::int64_t last_update_date = 0;


    /*! \class Resource SenderIdClass.h scg_api/SenderIdClass.h
     *
     * Resource class for SenderIdClass
     *
     */
    class Resource : public ResourceImpl<SenderIdClass>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<SenderIdClass>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/sender_id_classes")
        {
        }

        /*! List SenderIdClass
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *      - id
         *      - name
         *      - designation
         *      - applicable_countries
         *      - country_peak_throughput
         *      - country_daily_throughput
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

    };

    /*! Assign a data object to a resource.
     *
     * Can be used to distribute data objects among a pool of
     * resources, each using their own Scg instances and worker-threads.
     */
    void SetResource(Resource *res) noexcept { }

};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::SenderIdClass,
    (std::string, id)
    (std::string, name)
    (std::string, description)
    (std::string, designation)
    (std::string, applicable_countries)
    (std::int64_t, country_peak_throughput)
    (std::int64_t, country_peak_total_throughput)
    (std::int64_t, country_daily_throughput)
    (std::string, delivery_window)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date))
