// Complete example, showing how to list all sender-id types available.

// We want to set the log-level
#include "restc-cpp/logging.h"
#include "restc-cpp/typename.h"

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
        ("mdn,m", po::value<string>()->default_value("15550000001"), "Mdn")
        ("senderid,s", po::value<string>()->default_value(""), "Sender id")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    if (vm.count("help")) {
        cout << opts;
        return -1;
    }

    // Authentication file
    const auto auth_path = vm["auth"].as<string>();

    // URL to the API server
    const auto url = vm["url"].as<string>();

    // GSM number
    const auto mdn = vm["mdn"].as<string>();

    // Sender ID to use
    const auto senderid = vm["senderid"].as<string>();

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

        MessageRequest::Resource res(session);
        MessageRequest new_mrq;

        new_mrq.from = string("sender_id:") + senderid;
        new_mrq.to = {mdn};
        new_mrq.body = "Hello World";
        new_mrq.test_message_flag = true;

        auto id = res.Create(new_mrq);

        auto mrq = res.Get(id);

        // See if we get a message object fro this request.
        // Wait up to 60 seconds.
        bool got_messages = false;
        for(auto retry = 0; retry < 60; ++retry) {
            auto messages = mrq->ListMessages().ToContainer();
            if (!messages.empty()) {
                got_messages = true;
                break;
            }

            this_thread::sleep_for(1s);
        }

        for(const auto& msg : mrq->ListMessages()) {
            cout << "Message id " << msg.id << ": " << msg.state << endl;
        }

        mrq->Delete();
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

