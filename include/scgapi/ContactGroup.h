
#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


#include "scgapi/BaseData.h"
#include "scgapi/Contact.h"

namespace scg_api {

/*! \class ContactGroup ContactGroup.h scg_api/ContactGroup.h
 *
 * This resource can represent a collection of Contacts meant to
 * support bulk messaging operations (e.g. the ability to deliver
 * messages to multiple contacts via a single API call.) Contact
 * group can be represented as a fixed list of contacts or can be
 * represented as a specific criteria (filter) and each time the
 * group is referenced, SCG will compute the list of contacts that
 * match the criteria to define the contacts within the contact group.
 * Contact groups are owned by Clients and composed of Contacts.
 */
class ContactGroup : public BaseData
{
public:
    // Read only members
    /// [read only]
    std::string id;
    /// [read only] Processing, Ready
    std::string status;
    /// [read only] Count of members in the group.
    std::int64_t member_count = 0;
    /// [read only] The creation date of the object.
    std::int64_t created_date = 0;
    /// [read only] The last modification date of the object.
    std::int64_t last_update_date = 0;
    /*! [read only] The group type:
     *   - DYNAMIC meaning the list of members associated with the
     *          group is calculated dynamically whenever the group
     *          is referenced by applying the CRITERIA to the customer
     *          contact list
     *   - STATIC meaning the list of members is determined at the
     *          time of contact group creation and hence static.
     *          Customers will be also able to add or remove contacts
     *          from a STATIC group by accessing the contacts list
     *          associated with the group.
     */
    std::string type;

    // Special members
    int version_number = 0;

    // Read/write members
    /// Customer supplied id of the contact group
    std::string external_id;
    std::string name;
    std::string description;
    /*! Filter definition for creation of the group. Users can use
     * either criteria or members.
     */
    std::string criteria;
    /// The application id this contact belong to
    std::int64_t application_id = 0;

    /*! \internal */
    template <typename listT>
    struct AddContactsPayload {
        listT contacts;
    };

    /*! \class Resource ContactGroup.h scg_api/ContactGroup.h
     *
     * Resource class for ContactGroup
     *
     */
    class Resource : public ResourceImpl<ContactGroup>
    {
    public:
        using data_t = ContactGroup;

        Resource(Session& session)
        : ResourceImpl(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/contact_groups")
        {
        }

        /*! List ContactGroups
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *        - id
         *        - external_id
         *        - name
         *        - type
         *        - status
         *        - created_date
         *        - last_update_date
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a ContactGroup on the server.
         *
         * \arg obj ContactGroup object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const data_t& obj) {
            return Create_(obj);
        }

        /*! Update a ContactGroup on the server
         *
         * The ContactGroup instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const data_t& obj) {
            return Update_(obj);
        }

        /*! Delete a ContactGroup
         *
         * \arg id of the ContactGroups to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a ContactGroup object from the server.
         *
         * \arg id of the ContactGroup you want.
         *
         * \return unique pointer to a ContactGroup
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        template <typename listT>
        auto AddContact(listT list, const std::string& id) {
            auto url = GetContactsUrl(id);

            AddContactsPayload<listT> contacts_list;
            contacts_list.contacts = std::move(list);

            auto reply = DoPost<decltype(contacts_list)>(contacts_list, url);
        }

        /*! \internal */
        auto DeleteContact(const std::string& gid,
                           const std::string& cid) {
            auto url = GetContactsUrl(gid) + "/" + cid;
            return DeleteUrl_(url);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "status", "member_count", "type", "created_date",
                "last_update_date", "application_id"
            };

            return &names;
        }

        /*! \internal */
        std::string GetContactsUrl(const std::string& id) const {
            return GetResourceUrl() + "/" + id + "/contacts";
        }

        /*! \internal */
        auto ListContacts(const std::string& id,
                          const filter_t *filter = nullptr,
                          const ListParameters *lp = nullptr) {
            if (!contact_res_) {
                contact_res_ = std::make_unique<Contact::Resource>(
                    GetSession(), GetContactsUrl(id));
            }

            return contact_res_->List(filter, lp);
        }

    private:
        std::unique_ptr<Contact::Resource> contact_res_;
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

    /*! Update a ContactGroup on the server
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void Update() {
        VerifyForOperations();
        res_->Update(*this);
    }

    /*! Delete a ContactGroup on the server
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a ContactGroup if you have the ID using
     * the Resource::Delete method.
     */
    void Delete() {
        VerifyForOperations();
        res_->Delete(id);
    }

    /*! Add a contact to a STATIC group
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     */
    void AddContact(const Contact& contact) {
        assert(!contact.id.empty());
        AddContact(contact.id);
    }


    /*! Add a contact to a STATIC group
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     */
    void AddContact(const std::string& id) {
        std::vector<std::string> contacts
            = std::initializer_list<std::string>{id};
        AddContact<std::vector<std::string>>(std::move(contacts));
    }

    /*! Bulk add contacts.
     *
     * \arg list C++ container with contact objects.
     *      Typically a std::list<Contact> or std::vector<Contact>.
     */
    template <typename listT = std::vector<std::string>>
    void AddContact(listT contacts) {
        VerifyForOperations();
        res_->AddContact<listT>(std::move(contacts), id);
    }

    /*! Delete a contact to a STATIC group
     *
     * The Contact itself is not deleted, just it's
     * inclusion in this Contact Group.
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     */
    void DeleteContact(const Contact& contact) {
        VerifyForOperations();
        DeleteContact(contact.id);
    }

    /*! Delete a contact to a STATIC group
     *
     * The Contact itself is not deleted, just it's
     * inclusion in this Contact Group.
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     */
    void DeleteContact(const std::string& cid) {
        VerifyForOperations();
        res_->DeleteContact(id, cid);
    }

    /*! List contacts from this group.
     *
     * \arg filter Please see scgapi::Contact::Resource::List()
     * \arg lp List Parameters
     *
     * The ContactGroup instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * \returns Iterator to result set.
     *
     */
    auto ListContacts(const filter_t *filter = nullptr,
        const ListParameters *lp = nullptr) {
        VerifyForOperations();
        return res_->ListContacts(id, filter, lp);
    }

private:
    void VerifyForOperations() {
        VerifyForOperations_(id);
    }

    Resource *res_ = {};
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::ContactGroup,
    (std::string, id)
    (std::string, status)
    (std::int64_t, member_count)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (std::string, type)
    (int, version_number)
    (std::string, external_id)
    (std::string, name)
    (std::string, description)
    (std::string, criteria)
    (std::int64_t, application_id))


BOOST_FUSION_ADAPT_TPL_STRUCT(
    (listT),
    (scg_api::ContactGroup::AddContactsPayload)(listT),
    (typename BOOST_IDENTITY_TYPE((listT)), contacts))
