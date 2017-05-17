
# What is this?
This is the C++ version of the SCG API.
We have prepared simple to use C++ classes, representing the
different REST API interfaces.

The C++ API hides some of the REST API's constraints, like
lists being returned in logical pages of <i>n</i> records. With the
C++ SDK, the List() method returns an iterator that returns
items, until there are no more.

## How to use the SDK
All the data objects follow a common pattern.

You get a handle to a resource (like Contact or SenderId) by getting
an instance of <Object>::Resource(session).

To use the SDK, you create an instance of the Scg class, and then
call Connect() or the template version ConnectT<>.
Connect takes a functor as argument, and inside the functor,
you get an instance of a Session.

The C++ version of the SDK is asynchronous. It use co-routines to
simplify the code. If you are unfamiliar with boost::asio::coroutines,
yo can find a simple explanation here [Modern Async C++ Example](https://github.com/jgaa/modern_async_cpp_example).

After having a Session instance available, you need to instantiate
a Resource object for the class you want to work with.

The Resource classes
typically has <b>List()</b> and <b>Create()</b> methods (depending
on the actual REST API methods available for that data type).

To list the data objects for a resource, like your Contacts,
you can call List() without any arguments to get all the contacts,
or List(filter_t) to filter the result-set. List()
returns a container-like object that let's you iterate over the data-set
trough begin() and end() methods.

Note that in order to use methods on an object that is created from
a Resource, the Session instance for that resource must still be in
scope. This is because the objects keeps a reference to the Session,
and use this session to execute asynchronous requests to the API server
in the background. If you want to call methods on an object, after
it's session is out of scope, you need to Get() it again from a
Resource with a valid Session. See [send_sms_to_grp_async.cpp](examples/send_sms_to_grp_async.cpp)
for an example on this.

## Getting started

The SDK use CMake to build it's library. In order to use it,
you need:

- Boost
- OpenSSL
- Zlib

If you use a recent flavour of Linux, you should be able to use the
packaged development versions of these libraries. The We have tested
the SDK under Ubuntu zesty.

The SDK embed the [restc-cpp](https://github.com/jgaa/restc-cpp) C++ REST
Client Library as a git sub-module, and builds it with CMake. You need to
initialize and update the git sub-module(s) before you start compiling the
library.

```sh
$ git submodule update --init --recursive
```

To compile the project, from the projects top directory:
```sh
mkdir build
cd build
cmake ..
make
cd ..
```


# Some simple examples

## List Sender ID Types

```C++
    auto future = scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        // Instatiate a Resource for the SenderIdType
        SenderIdType::Resource res(session);

        // Fetch a pseudo-container with the list of all SenderIdType objects
        auto list = res.List();

        // Iterate over the returned objects.
        for(const auto & sit : list) {
            RESTC_CPP_LOG_INFO << "SenderIdType: id = " << sit.id
            << ", name = " << sit.name;
        }

        // End of lambda.

        // Note that the code above actually executes asynchronously.
        // You can start thousands of such routines from one scg instance,
        // and they will all share the same worker-thread, releasing
        // the thread to other instances when they wait for IO, resolves
        // dns names etc.
    });
```
[Full example](examples/list_senderid_types.cpp)

The result from the example program above may look like:
```
[2017-05-12 19:06:23.917694] [0x00007f40b5b67700] [info]    SenderIdType: id = SHORTCODE, name = Short code
[2017-05-12 19:06:23.917750] [0x00007f40b5b67700] [info]    SenderIdType: id = LONGCODE, name = Long code
[2017-05-12 19:06:23.917760] [0x00007f40b5b67700] [info]    SenderIdType: id = EMAIL, name = Email
[2017-05-12 19:06:23.917768] [0x00007f40b5b67700] [info]    SenderIdType: id = PUSH, name = Push
[2017-05-12 19:06:23.917775] [0x00007f40b5b67700] [info]    SenderIdType: id = FACEBOOK, name = Facebook
[2017-05-12 19:06:23.917783] [0x00007f40b5b67700] [info]    SenderIdType: id = WECHAT, name = WeChat
[2017-05-12 19:06:23.917790] [0x00007f40b5b67700] [info]    SenderIdType: id = TOLLFREE, name = Toll free code
```

You can create an object by calling Create() with an object (template)
of the data type you aim to create, with the relevant members assigned
valid values.


```C++
    scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        // Instatiate a Resource for Contact
        Contact::Resource res(session);

        // Instatiate a template with the properties for the new Contact
        Contact contact_tpl;
        contact_tpl.first_name = "John";
        contact_tpl.last_name = "Doe";
        contact_tpl.primary_mdn = mdn; // mdn is a string with the GSM number defined elsewhere

        // Create the contact
        // Note that Create returns an object id, not an object instance.
        const string contact_id = res.Create(contact_tpl);

        // Get an instance of the object.
        auto contact = res.Get(contact_id);
        cout << "Created contact with id: " << contact->id << endl;

        // What data-type is 'contact'?
        cout << "contact is of type: "
            << RESTC_CPP_TYPENAME(decltype(contact)) << endl;

        contact->Delete();
    });
```
[Full example](examples/create_contact.cpp)

The example above may give output like:
```
Created contact with id: rBnuqAc3w9rKanTM5pKCh5
contact is of type: std::unique_ptr<scg_api::Contact, std::default_delete<scg_api::Contact> >
```

All objects that can be updated or deleted has <b>Update()</b> and/or
<b>Delete()</b> methods. The resource of an object also have <b>Delete()</b>
methods, so if you need to delete an object you just know by it's id,
there is no need to instatiate it. You jst call:

```C++
    res.Delete(id)
```

Some objects has methods that let you add/query or delete
other objects or references it holds to other objects. For
example, when you create a message-request to a group of
contacts, you may generate a large number of message objects.
These can be iterated over from MessageRequest.listMessages()

```C++
    // mrq is an instance of a MessageRequest that has created message(s).
    for(const auto& msg : mrq->ListMessages()) {
        cout << "Message id " << msg.id << ": " << msg.state << endl;
    }
```

The output of the code snipplet above may be:
```
Message id 6imkYtXxcTUge4l1SyyKD1: SENT
Message id 3w6vWG8zbP9LNXprvwB2T: SENT
```

## Error handling
Errors are reported trough exceptions derived from std::exception

# Some more examples

## Listing Sender Id's
If you want to list available Sender Id's, it can be done as easy as:

```C++
// Operator that can list capabilities
ostream& operator << (ostream& o, const vector<string>& v) {
    for(const auto& s: v) {
        o << '[' << s << "] ";
    }
    return o;
}
```

```C++
    scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        SenderId::Resource res(session);

        filter_t filter = {
            {"class_id", "COMMERCIAL"},
            {"state", "ACTIVE"},
            {"ownership", "PRIVATE"},
            {"country", "USA"}
        };

        for(const auto& sid : res.List(&filter)) {
            cout << "Senderid: " << sid.id
                << ", capabilities: " << sid.capabilities
                << endl;
        }
    });

```
[Full example](examples/list_senderid.cpp)

This should produce output like:
```text
Senderid: mJG1QDXwl9XPm6K4wvfkd5, capabilities: [MMS] [SMS]
Senderid: ln9sk9JF6insXcJ5nUzKK3, capabilities: [SMS]
Senderid: AE0vtyghu8dIrrpXesXPK1, capabilities: [MMS] [SMS]
Senderid: dY2GycQpj6OE1x2mH1Ezc6, capabilities: [VOICE]
Senderid: KdNRN6dl5IkwIH5B829XJ2, capabilities: [MMS] [SMS]
Senderid: tcS8h40LXgvJsGMQI93WK4, capabilities: [MMS] [SMS]
Senderid: WeUt68X4X3soefNuYm17q5, capabilities: [MMS] [SMS]
Senderid: anaKnQLX5fgeXo6bgl6r54, capabilities: [MMS] [SMS]

```

## Sending a SMS to a GSM number
```C++
    scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        MessageRequest::Resource res(session);
        MessageRequest new_mrq;

        new_mrq.from = "sender_id:"s + senderid;
        new_mrq.to = {mdn};
        new_mrq.body = "Hello World";
        const auto msgreq_id = res.Create(new_mrq);

        cout << "Created Message Request: " << msgreq_id << endl;
    });
```
[Full example](examples/send_sms.cpp)


where the settings are changed to a real SenderId and GSM number.

This should produce output similar to:
```text
Sent message request aQWY9PeMCO01TEH9bk1ek5
```

## Sending a Message to a Contact

This works as above, except for the to field in Create()
```C++
    String contactId = "<Id of an existing contact>";
    ...
    new_mrq.to = {"contact:"s + contactId};
    ...
```

## Sending a Message to a Group

Here we will create two new contacts, a new group, assign the contacts
to the group, and then send a message to the group.

```C++
    auto future = scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        // Instantiate a Resource for Contact
        Contact::Resource contact_res(session);

        // Instantiate a template with the properties for the new Contact
        Contact contact_tpl;

        // Create Bob
        contact_tpl.first_name = "Bob";
        contact_tpl.primary_mdn = to_string(mdn++);
        const string bob_id = contact_res.Create(contact_tpl);

        // Create Alice
        contact_tpl.first_name = "Alice";
        contact_tpl.primary_mdn = to_string(mdn++);
        const string alice_id = contact_res.Create(contact_tpl);

        ContactGroup::Resource grp_res(session);
         // Instantiate a template with the properties for the new Contact
        ContactGroup group_tpl;
        group_tpl.name = "Our Friends";

        // Create the Group
        const string friends_id = grp_res.Create(group_tpl);

        // Instantiate the group
        auto friends = grp_res.Get(friends_id);

        // Add our friends
        friends->AddContact(bob_id);
        friends->AddContact(alice_id);

        // Create a message request resource
        MessageRequest::Resource mrq_res(session);

        // Create a template for creating a new message request
        MessageRequest new_mrq;

        // Assign values
        new_mrq.from = "sender_id:"s + senderid;
        // Address the message to the group
        new_mrq.to = {"group:"s + friends->id};
        new_mrq.body = "Hello World";
        new_mrq.test_message_flag = true;
        const auto msgreq_id = mrq_res.Create(new_mrq);

        cout << "Created Message Request: " << msgreq_id << endl;
    });
```
[Full example](examples/send_sms_to_grp.cpp)

[Alternative example, creating objects in parallell](examples/send_sms_to_grp_async.cpp)


## Sending a MMS with an attachment

```C++
    auto future = scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        // Create a new attachment
        Attachment::Resource att_res(session);

        Attachment new_att;
        new_att.name = "test_upload";
        new_att.type = "image/jpeg";
        new_att.filename = "cute-cat.jpg";
        const auto att_id = att_res.Create(new_att);
        auto attachment = att_res.Get(att_id);

        // Upload the file we want to attach to the MMS
        // For the simplicity of this example, we assume
        // that this is a jpeg image of a cute cat.
        attachment->UploadContent(att_path);

        // Now, send a message request. This is just like sending
        // an SMS, secept that we add the attachment.
        MessageRequest::Resource res(session);
        MessageRequest new_mrq;

        new_mrq.from = "sender_id:"s + senderid;
        new_mrq.to = {mdn};
        new_mrq.body = "Hello World";
        new_mrq.attachments.push_back(attachment->id);
        new_mrq.test_message_flag = test_flag;
        const auto msgreq_id = res.Create(new_mrq);

        cout << "Created Message Request: " << msgreq_id << endl;
    });

```
[Full example](examples/send_mms.cpp)


This should produce output similar to:
```
Created Message Request: l1hTmXHNKlSWc8SrpHXNS2
```

## Checking the state of a Message Request
```C++
    auto future = scg->Connect(url, auth, [&](Session& session) {
        // This is a C++ lambda co-routine, running in a worker-thread.

        MessageRequest::Resource res(session);

        auto mrq = res.Get(mrqid);

        cout << "Message Request " << mrq->id
            << " is in state " << mrq->state
            << " with " << mrq->delivered_count
            << " delivered and " << mrq->failed_count
            << " failed messages."
            << endl;

        for(const auto& msg : mrq->ListMessages()) {
            cout << " - Message " << msg.id
                << " is in state " << msg.state
                << ", error code: " << msg.failure_code
                << ", error reason: " << msg.failure_details
            << endl;
        }
    });
```
[Full example](examples/check_message_request_state.cpp)


This should produce output similar to:
```
Message Request l1hTmXHNKlSWc8SrpHXNS2 is in state COMPLETED with 0 delivered and 0 failed messages.
 - Message qrT6L69Vfj0n0xXfFo37R6 is in state DELIVERED, error code: , error reason:
```


