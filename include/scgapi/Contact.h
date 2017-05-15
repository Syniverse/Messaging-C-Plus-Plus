#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"

namespace scg_api {

/*! \class Contact Contact.h scg_api/Contact.h
 *
 * A Contact represents a person/application/entity which the SCG
 * communicates with on behalf of applications. Contacts do not send
 * or receive messages directly via the Syniverse Messaging Gateway,
 * but rather do so directly via the media channel. The distinction
 * between Applications and Contacts is thus based on how the
 * messaging is being performed. Applications (entities using the
 * APIs to initiate messaging) send/receive messages from Contacts
 * (entities which are not using the APIs to initiate messaging.)
 */
class Contact : public BaseData
{
public:
    using fast_access_map_t = std::map<std::string, std::string>;

    class ApplicationToken : public BaseData
    {
    public:
        ApplicationToken() = default;

        std::string message_delivery_provider;
        std::string sender_id_address;
        std::string token;
        int version_number = 0;
        std::string id;
        std::int64_t application_id = 0;
        std::int64_t created_date = 0;
        std::int64_t last_update_date = 0;

       /*! \class Resource Contact.h scg_api/Contact.h
        *
        * Resource class for ApplicationToken
        *
        */
        class Resource : public ResourceImpl<ApplicationToken>
        {
        public:
            Resource(Session& session, const std::string& contactId)
            : ResourceImpl<ApplicationToken>(
                session,
                session.GetUrl()
                    + "/scg-external-api/api/v1/contacts/"
                    + contactId
                    + "/application_tokens")
            {
            }


            /*! List ApplicationTokens
            *
            * \arg filter Key/Value pairs of attributes that can
            *      filter the result-set.
            *
            * \arg lp List Parameters
            *
            * \returns Iterator to the result-set.
            */
            auto List(const filter_t *filter = nullptr,
                const ListParameters *lp = nullptr) {
                return List_(filter, lp);
            }

            /*! Create an instance of a Contact on the server.
            *
            * \arg obj Contact object where the relevant data-
            *      members are set to valid values.
            * \returns id of the new object.
            */
            auto Create(const ApplicationToken& at) {
                return Create_(at);
            }


            /*! Delete a Contact
            *
            * \arg id of the Contact to delete on the server.
            */
            auto Delete(const std::string& id) {
                return Delete_(id);
            }

            /*! Get a Contact object from the server.
            *
            * \arg id of the Contact you want.
            *
            * \return unique pointer to a Contact
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
                    "last_update_date"};

                return &names;
            }
        };

       /*! Assign a data object to a resource.
        *
        * Can be used to distribute data objects among a pool of
        * resources, each using their own Scg instances and worker-threads.
        */
        void SetResource(Resource *res) noexcept { res_ = res; }


        /*! Delete a ApplicationToken on the server
        *
        * The ApplicationToken instance must be received
        * from the server. You cannot delete an instance
        * you have instantiated yourself.
        *
        * You can delete a ApplicationToken if you have the ID using
        * the Resource::Delete method.
        */
        void Delete() {
            VerifyForOperations();
            res_->Delete(id);
        }

    private:
        void VerifyForOperations() {
            if (!res_ || id.empty())
                throw std::runtime_error(
                    "This object is not initialized for operations");
        }

        Resource *res_ = nullptr;
     };


    class AccessToken : public BaseData
    {
    public:
        AccessToken() = default;

        int duration = 0;
        int version_number = 0;
        std::string id;
        std::int64_t expiry_time = 0;
        std::int64_t application_id = 0;
        std::int64_t created_date = 0;
        std::int64_t last_update_date = 0;

        /*! \class Resource Contact.h scg_api/Contact.h
        *
        * Resource class for AccessToken
        *
        */
        class Resource : public ResourceImpl<AccessToken>
        {
        public:
            Resource(Session& session, const std::string& contactId)
            : ResourceImpl<AccessToken>(
                session,
                session.GetUrl()
                    + "/scg-external-api/api/v1/contacts/"
                    + contactId
                    + "/access_tokens")
            {
            }


            /*! List AccessTokens
            *
            * \arg filter Key/Value pairs of attributes that can
            *      filter the result-set.
            *
            * \arg lp List Parameters
            *
            * \returns Iterator to the result-set.
            */
            auto List(const filter_t *filter = nullptr,
                const ListParameters *lp = nullptr) {
                return List_(filter, lp);
            }

            /*! Create an instance of a Contact on the server.
            *
            * \arg obj Contact object where the relevant data-
            *      members are set to valid values.
            * \returns id of the new object.
            */
            auto Create(const AccessToken& at) {
                return Create_(at);
            }


            /*! Delete a Contact
            *
            * \arg id of the Contact to delete on the server.
            */
            auto Delete(const std::string& id) {
                return Delete_(id);
            }

            /*! Get a Contact object from the server.
            *
            * \arg id of the Contact you want.
            *
            * \return unique pointer to a Contact
            *
            */
            auto Get(const std::string& id) {
                return Get_(id);
            }

            /*! \internal */
            const std::set< std::string > *
            GetReadOnlyNames() override {
                static const std::set<std::string> names = {
                    "id", "expiry_time", "application_id",
                    "created_date", "last_update_date"};

                return &names;
            }
        };

       /*! Assign a data object to a resource.
        *
        * Can be used to distribute data objects among a pool of
        * resources, each using their own Scg instances and worker-threads.
        */
        void SetResource(Resource *res) noexcept { res_ = res; }


        /*! Delete a AccessToken on the server
        *
        * The AccessToken instance must be received
        * from the server. You cannot delete an instance
        * you have instantiated yourself.
        *
        * You can delete a AccessToken if you have the ID using
        * the Resource::Delete method.
        */
        void Delete() {
            VerifyForOperations();
            res_->Delete(id);
        }

    private:
        void VerifyForOperations() {
            if (!res_ || id.empty())
                throw std::runtime_error(
                    "This object is not initialized for operations");
        }

        Resource *res_ = nullptr;
    };


    /*! \class Address Contact.h scg_api/Contact.h
     *
     *  all known addresses of the customer
     */
    struct Address {
        /// priority of the address, 1 highest
        int priority = 0;
        /// specifies type of the address- home, work
        std::string designation;
        /// billing, shipping
        std::string use;
        /// specifies where the address comes from, company specific
        std::string source;
        /// status of the address- VALID_NEW, VALID_CONF, INVALID, EXPIRED
        std::string status;
        /// first address line
        std::string line1;
        /// second address line
        std::string line2;
        std::string city;
        std::string state;
        std::string province;
        std::string zip;
        std::string country;
    };

    /*! \class Account Contact.h scg_api/Contact.h
     *
     *  all known accounts of the customer
     */
    struct Account {
        /// user defined account priority
        std::string priority;
        /// primary designation of the account- work, private …
        std::string designation;
        /// specifies where the address comes from, company specific
        std::string source;
        /// status of the address- VALID_NEW, VALID_CONF, INVALID, EXPIRED
        std::string state;
        /// email, social, web, loyalty
        std::string username;
        /// account domain. For example starwood.com
        std::string domain;
        /*! the access token to the account where the account requires
         * some oauth type authorization
         */
        std::string access_token;
    };

    /*! \class Device Contact.h scg_api/Contact.h
     *
     * all known customer devices
     */
    struct Device {
        /// user defined account priority
        std::string priority;
        /// primary designation of the account- work, private
        std::string designation;
        /// specifies where the address comes from, company specific
        std::string source;
        /// status of the address- VALID_NEW, VALID_CONF, INVALID, EXPIRED
        std::string state;
        std::string msisdn;
        std::string carrier;
        std::string mac_address;
        std::string uuid;
        /// device manufacturer, for example Apple
        std::string manufacturer;
        /// model of the device, for example iPhone 5s
        std::string model;
        /// operating system of the device, for example iOS 8.3
        std::string os;
    };

    /*! \class Demographic Contact.h scg_api/Contact.h
     *
     * all known customer demographic
     */
    struct Demographic {
        /// demographic name
        std::string name;
        /// where the information was obtained
        std::string source;
        /// how likely the information is, floating point value 0...1
        std::string score;
    };

    /*! \class Interest Contact.h scg_api/Contact.h
     *
     *  all known customer preferences
     */
    struct Interest {
        /// interest code
        std::string code;
        /// interest name
        std::string name;
        ///  where the information was obtained
        std::string source;
        /// how likely the information is, floating point value 0...1
        std::string score;
    };

    // Read only
    /*! [read only] Unique identifier, allocated by SMG at the time
     * of contact creation
     */
    std::string id;
    /// [read only] The date the resource was created
    std::int64_t created_date = 0;
    /// [read only] The data the resource was last changed
    std::int64_t last_update_date = 0;

    // Special
    /*! An ascending version number of the resource. 1 upon creation,
     * incremented by one for each modification done. Used to
     * implement optimisitic locking.
     *
     * \note This is handled in the background by this SDK.
     */
    int version_number = 0;

    // Read / Write
    /*! the id of the contact at the customer system. Can be used by
     * customer to update the contacts.
     */
    std::string external_id;
    /// First name of customer
    std::string first_name;
    ///  Last name of customer
    std::string last_name;
    /*! Customer birth date.
     *
     * \todo: Document format
     */
    std::string birth_date;
    /*! The timestamp of the first customer acquisition - i.e. the
     * date when the customer has been for the first time opted
     * in to any program.
     */
    std::int64_t first_acquisition_date;
    /*! The timestamp of the last customer acquisition i.e. the date
     * when the customer has for the last time sent an optin keyword
     * or performed a doubleoptin
     */
    std::int64_t last_acquisition_date;
    /*! The primary phone number of the customer. This is the number
     * used to track the double optin for the customer.
     *
     * \note Tis fiels is required when creating new objects.
     */
    std::string primary_mdn;
    std::string primary_addr_line1;
    std::string primary_addr_line2;
    std::string primary_addr_city;
    std::string primary_addr_zip;
    std::string primary_addr_state;
    std::string primary_email_addr;
    std::string primary_social_handle;
    std::vector<Address> address_list;
    std::vector<Account> account_list;
    std::vector<Device> device_list;
    std::vector<Interest> interest_list;
    std::vector<Demographic> demographic_list;
    /*! JSON string of all extended attributes of the customer. Any
     * list of key-value pairs is supported. Also composite attributes
     * can be stored.
     */
    std::string extended_attributes;

    /// The application id this contact belong to
    std::int64_t application_id = 0;
    /** Voice preference
     *
     *  - NONE
     *  - PREFER_VOICE
     *  - REQUIRE_VOICE
     */
    std::string voice_preference;
    /// iso639-1 language code
    std::string preferred_language;
    /// Social handles
    std::vector<std::string> social_handles;

    // Fast access
    fast_access_map_t fast_access;
    std::string fast_access_1;
    std::string fast_access_2;
    std::string fast_access_3;
    std::string fast_access_4;
    std::string fast_access_5;
    std::string fast_access_6;
    std::string fast_access_7;
    std::string fast_access_8;
    std::string fast_access_9;
    std::string fast_access_10;
    std::string fast_access_11;
    std::string fast_access_12;
    std::string fast_access_13;
    std::string fast_access_14;
    std::string fast_access_15;
    std::string fast_access_16;
    std::string fast_access_17;
    std::string fast_access_18;
    std::string fast_access_19;
    std::string fast_access_20;


    std::shared_ptr<ApplicationToken::Resource>
    GetApplicationTokenResource() {
        VerifyForOperations();
        return std::make_shared<ApplicationToken::Resource>(
            res_->GetSession(), id);
    }

    std::shared_ptr<AccessToken::Resource>
    GetAccessTokenResource() {
        VerifyForOperations();
        return std::make_shared<AccessToken::Resource>(
            res_->GetSession(), id);
    }

    /*! \class Resource Contact.h scg_api/Contact.h
     *
     * Resource class for Contact
     *
     */
    class Resource : public ResourceImpl<Contact>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<Contact>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/contacts")
        {
        }

        /*! \internal
         *
         * Used by other classes that list customers as part of their
         * function.
         */
        Resource(Session& session, const std::string& url)
        : ResourceImpl<Contact>(session, url)
        {
        }

        /*! List Contacts
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *        - id
         *        - external_id
         *        - first_name
         *        - last_name
         *        - birth_date
         *        - first_acquisition_date
         *        - last_acquisition_date
         *        - primary_mdn
         *        - primary_addr_line1
         *        - primary_addr_line2
         *        - primary_addr_city
         *        - primary_addr_zip
         *        - primary_addr_state
         *        - primary_email_addr
         *        - primary_social_handle
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

        /*! Create an instance of a Contact on the server.
         *
         * \arg obj Contact object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const Contact& ch) {
            return Create_(ch);
        }

        /*! Update a Contact on the server
         *
         * The Contact instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const Contact& ch) {
            return Update_(ch);
        }

        /*! Delete a Contact
         *
         * \arg id of the Contact to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a Contact object from the server.
         *
         * \arg id of the Contact you want.
         *
         * \return unique pointer to a Contact
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

    /*! Update a Contact on the server
     *
     * The Contact instance must be receivedApplicationToken
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a Contact on the server
     *
     * The Contact instance must be received
     * from the server. You cannot delete an instance
     * you have instantiated yourself.
     *
     * You can delete a Contact if you have the ID using
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
    scg_api::Contact::ApplicationToken,
    (std::string, message_delivery_provider)
    (std::string, sender_id_address)
    (std::string, token)
    (int, version_number)
    (std::string, id)
    (std::int64_t, application_id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact::AccessToken,
    (int, duration)
    (int, version_number)
    (std::string, id)
    (std::int64_t, expiry_time)
    (std::int64_t, application_id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact::Address,
    (int, priority)
    (std::string, designation)
    (std::string, use)
    (std::string, source)
    (std::string, status)
    (std::string, line1)
    (std::string, line2)
    (std::string, city)
    (std::string, state)
    (std::string, province)
    (std::string, zip)
    (std::string, country))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact::Account,
    (std::string, priority)
    (std::string, designation)
    (std::string, source)
    (std::string, state)
    (std::string, username)
    (std::string, domain)
    (std::string, access_token))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact::Device,
    (std::string, priority)
    (std::string, designation)
    (std::string, source)
    (std::string, state)
    (std::string, msisdn)
    (std::string, carrier)
    (std::string, mac_address)
    (std::string, uuid)
    (std::string, manufacturer)
    (std::string, model)
    (std::string, os))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact::Demographic,
    (std::string, name)
    (std::string, source)
    (std::string, score))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact::Interest,
    (std::string, code)
    (std::string, name)
    (std::string, source)
    (std::string, score))

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Contact,
    (std::string, id)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (int, version_number)
    (std::string, external_id)
    (std::string, first_name)
    (std::string, last_name)
    (std::string, birth_date)
    (std::int64_t, first_acquisition_date)
    (std::int64_t, last_acquisition_date)
    (std::string, primary_mdn)
    (std::string, primary_addr_line1)
    (std::string, primary_addr_line2)
    (std::string, primary_addr_city)
    (std::string, primary_addr_zip)
    (std::string, primary_addr_state)
    (std::string, primary_email_addr)
    (std::string, primary_social_handle)
    (std::vector<scg_api::Contact::Address>, address_list)
    (std::vector<scg_api::Contact::Account>, account_list)
    (std::vector<scg_api::Contact::Device>, device_list)
    (std::vector<scg_api::Contact::Interest>, interest_list)
    (std::vector<scg_api::Contact::Demographic>, demographic_list)
    (std::string, extended_attributes)
    (std::int64_t, application_id)
    (std::string, voice_preference)
    (std::string, preferred_language)
    (std::vector<std::string>, social_handles)
    (scg_api::Contact::fast_access_map_t, fast_access)
    (std::string, fast_access_1)
    (std::string, fast_access_2)
    (std::string, fast_access_3)
    (std::string, fast_access_4)
    (std::string, fast_access_5)
    (std::string, fast_access_6)
    (std::string, fast_access_7)
    (std::string, fast_access_8)
    (std::string, fast_access_9)
    (std::string, fast_access_10)
    (std::string, fast_access_11)
    (std::string, fast_access_12)
    (std::string, fast_access_13)
    (std::string, fast_access_14)
    (std::string, fast_access_15)
    (std::string, fast_access_16)
    (std::string, fast_access_17)
    (std::string, fast_access_18)
    (std::string, fast_access_19)
    (std::string, fast_access_20))

