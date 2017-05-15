// Complete example, showing how to list all sender-id types available.

#include <atomic>

// We want to set the log-level
#include "restc-cpp/logging.h"

// Include some boiler-plate boost headers
#include <boost/program_options.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/filesystem.hpp>

// Include the required SDK headers
#include "scgapi/Scg.h"
#include "scgapi/Contact.h"
#include "scgapi/ContactGroup.h"
#include "scgapi/Message.h"
#include "scgapi/MessageRequest.h"

// Best practice is to not clobber the code with name spaces
using namespace std;
using namespace scg_api;
using namespace restc_cpp;

int main(int argc, char * argv[])
{
    // Parse the command-line
    namespace po = boost::program_options;
    po::options_description opts("Options");

    opts.add_options()
        ("help,h", "Show help")
        ("auth,a", po::value<string>()->default_value("auth.json"), "Json auth file")
        ("url,u", po::value<string>()->default_value("https://beta.api.syniverse.co"), "URL to api server")
        ("test,t", po::value<bool>()->default_value(true), "Test message flag")
        ("mdn,m", po::value<int64_t>()->required(), "Mdn (start of sequence)")
        ("senderid,s", po::value<string>()->required(), "Sender id")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    if (vm.count("help")) {
        cout << opts;
        return -1;
    }

    try {
        po::notify(vm);
    } catch(const po::required_option& ex) {
        cerr << "Error: " << ex.what() << endl;
        return -1;
    }

    // Authentication file
    const auto auth_path = vm["auth"].as<string>();

    // URL to the API server
    const auto url = vm["url"].as<string>();

    // Test flag. If set, the message will be processed but not sent
    const auto test_flag = vm["test"].as<bool>();

    // GSM number
    std::atomic<int64_t> mdn;
    mdn = vm["mdn"].as<int64_t>();

    // Sender ID to use
    const auto senderid = vm["senderid"].as<string>();

    // Set the log level
    namespace logging = boost::log;
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::trace
    );

    // Use the SDK's log macros to give some status information
    RESTC_CPP_LOG_DEBUG << "Example starting in " << boost::filesystem::current_path();
    RESTC_CPP_LOG_DEBUG << "Using auth-file: " << auth_path;


    // Instantiate an object with the authentication info
    auto auth = make_shared<scg_api::AuthInfo>(auth_path);

    // Create an instance of Scg;
    auto scg = Scg::Create();

    // Create a co-routine for the job
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
        new_mrq.test_message_flag = test_flag;
        const auto msgreq_id = mrq_res.Create(new_mrq);

        cout << "Created Message Request: " << msgreq_id << endl;

        // It's good to delete the resources here in the example, but you
        // probably don't want to do that in your own code, as the messages
        // are unlikely to be sent when the group and / or contacts don't
        // exist at the time the server process the messages.
        friends->Delete();

        // Since bob and alice's sessions are out of scope,
        // we use a resource instance with the current session
        // to delete them.
        contact_res.Delete(bob_id);
        contact_res.Delete(alice_id);
    });

    // Use a try-block to catch exceptions that may be re-thrown from the
    // lambda block when you call the future's get() method.
    try {
        // Wait for the last lambda to finish it's execution.
        future.get();
    } catch(const exception& ex) {
        cerr << "Execution failed with exception: " << ex.what() << endl;
    }
}

