
#pragma once

#include <set>

#include <boost/fusion/adapted.hpp>

#include "scgapi/BaseData.h"


namespace scg_api {

/*! \class Attachment Attachment.h scg_api/Attachment.h
 *
 * The resource used to store attachment meta-data and the Attachment
 * ID that is used to upload and download the attachment content.
 *
 */
class Attachment : public BaseData
{
public:
    // Read Only
    /// [read only] Unique identifier
    std::string id;
    /*! [read only]  ID of the application that has created the
     * attachment. May not be present on attachments on MO messages
     */
    std::int64_t application_id = 0;
    /// [read only] CREATED, UPLOADED
    std::string state;
    /*! [read only] For a MT message (one which was sent via this API), this
     * indicates the time when the API was called to request message
     * delivery. For a MO message, this is the point in time when the
     * SMG was notified of the inbound message from the operator
     * network/mediation channel.
     */
    std::int64_t created_date = 0;
    /// [read only] The data the resource was last changed
    std::int64_t last_update_date = 0;

    // Special
    int version_number = 0;

    // Read / Write
    /// Original file name of the attachment
    std::string filename;
    /*! The name of the attachment (e.g. it’s filename or other
     * non-generated identifier) The value of this field should be
     * unique for a given message.
     */
    std::string name;
    /// The MIME type of the attachment (if known)
    std::string type;
    /*! The size, in bytes, of the attachment body
     *
     * This will be filled in automatically by the server when the
     * attachment is uploaded.
     */
    int size = 0;

    /*! \class Resource Attachment.h scg_api/Attachment.h
     *
     * Resource class for Attachment
     *
     */
    class Resource : public ResourceImpl<Attachment>
    {
    public:
        Resource(Session& session)
        : ResourceImpl<Attachment>(
            session,
            session.GetUrl() + "/scg-external-api/api/v1/messaging/attachments")
        {
        }

        /*! List Attachments
         *
         * \arg filter Key/Value pairs of attributes that can
         *      filter the result-set.
         *         - id
         *         - application_id
         *         - name
         *         - type
         *         - size
         *         - filename
         *         - state
         *
         * \arg lp List Parameters
         *
         * \returns Iterator to the result-set.
         */
        auto List(const filter_t *filter = nullptr,
            const ListParameters *lp = nullptr) {
            return List_(filter, lp);
        }

        /*! Create an instance of a Attachment on the server.
         *
         * \arg obj Attachment object where the relevant data-
         *      members are set to valid values.
         * \returns id of the new object.
         */
        auto Create(const Attachment& obj) {
            return Create_(obj);
        }

        /*! Update a Attachment on the server
         *
         * The Attachment instance must be received
         * from the server. You cannot update an instance
         * you have instantiated yourself.
         */
        auto Update(const Attachment& obj) {
            return Update_(obj);
        }

        /*! Delete a Attachment
         *
         * \arg id of the Attachments to delete on the server.
         */
        auto Delete(const std::string& id) {
            return Delete_(id);
        }

        /*! Get a Attachment object from the server.
         *
         * \arg id of the Attachment you want.
         *
         * \return unique pointer to a Attachment
         *
         */
        auto Get(const std::string& id) {
            return Get_(id);
        }

        /*! \internal */
        auto GetFileUrlWithToken(const std::string& id) {
            auto url = GetResourceUrl() + "/" + id + "/access_tokens";
            GenericReply token;
            restc_cpp::SerializeFromJson(token,
                                         DoPostNoBody(url, {}));

            return GetSession().GetUrl()
                + "/scg-attachment/api/v1/messaging/attachments/"
                + token.id + "/content";
        }

        /*! \internal */
        void UploadContent(const std::string& id,
                           const boost::filesystem::path& path,
                           const std::string& suggestedFileName,
                           const std::string& mimeType) {
            std::string url = GetFileUrlWithToken(id);

            auto reply = UploadFile_(
                url, path, suggestedFileName, mimeType);
        }

        /*! \internal */
        void DownloadContent(const std::string& id,
                             const boost::filesystem::path& path) {
            std::string url = GetFileUrlWithToken(id);

            DownloadFile_(url, path);
        }

        /*! \internal */
        const std::set< std::string > *
        GetReadOnlyNames() override {
            static const std::set<std::string> names = {
                "id", "application_id", "state", "created_date",
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

    /*! Change the name of the attachment on the server
     *
     * The Attachment instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     */
    void ChangeName(const std::string& name) {
        VerifyForOperations();
        Attachment tmp;
        tmp.id = id;
        tmp.version_number = version_number;
        tmp.name = name;
        res_->Update(tmp);
    }

    /*! Delete a Attachment on the server
     *
     * The Attachment instance must be received
     * from the server. You cannot update an instance
     * you have instantiated yourself.
     *
     * You can delete a Attachment if you have the ID using
     * the Resource::Delete method.
     */
    void Delete() {
        VerifyForOperations();
        res_->Delete(id);
    }

    /*! Upload a file as the content of the attachment */
    void UploadContent(const boost::filesystem::path& path) {
        VerifyForOperations();

        res_->UploadContent(id, path, filename, "");
    }

    /*! Download the content of the attachment as a file */
    void DownloadContent(const boost::filesystem::path& path) {
        VerifyForOperations();

        res_->DownloadContent(id, path);
    }

private:
    void VerifyForOperations() {
        VerifyForOperations_(id);
    }

    Resource *res_ = {};
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    scg_api::Attachment,
    (std::string, id)
    (std::int64_t, application_id)
    (std::string, state)
    (std::int64_t, created_date)
    (std::int64_t, last_update_date)
    (int, version_number)
    (std::string, filename)
    (std::string, name)
    (std::string, type)
    (int, size))


