// Complete example, showing how to list all sender-id types available.

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
#include "scgapi/Message.h"
#include "scgapi/MessageRequest.h"
#include "scgapi/Attachment.h"

// Best practice is to not clobber the code with name spaces
using namespace std;
using namespace scg_api;

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
        ("mdn,m", po::value<string>()->required(), "Mdn")
        ("senderid,s", po::value<string>()->required(), "Sender id")
        ("file-attachment,f", po::value<string>()->required(), "attachment to send, in jpeg format")
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
    const auto mdn = vm["mdn"].as<string>();

    // Sender ID to use
    const auto senderid = vm["senderid"].as<string>();

    const boost::filesystem::path att_path = vm["file-attachment"].as<string>();

    // Set the log level
    namespace logging = boost::log;
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );

    // Use the SDK's log macros to give some status information
    RESTC_CPP_LOG_DEBUG << "Example starting in " << boost::filesystem::current_path();
    RESTC_CPP_LOG_DEBUG << "Using auth-file: " << auth_path;


    // Instatiate an object with the authentication info
    auto auth = make_shared<scg_api::AuthInfo>(auth_path);

    // Create an instance of Scg;
    auto scg = Scg::Create();

    // Create a co-routine that can use the SDK, connecting to 'url'
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

    // Use a try-block to catch exceptions that may be re-thrown from the
    // lambda block when you call the future's get() method.
    try {
        // Wait for the lambda to finish it's execution.
        future.get();
    } catch(const exception& ex) {
        cerr << "Execution failed with exception: " << ex.what() << endl;
    }
}

