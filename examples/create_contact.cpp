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
#include "scgapi/Contact.h"

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

    string mdn = "3598950000001";

    // Create a co-routine that can use the SDK, connecting to 'url'
    auto future = scg->Connect(url, auth, [&](Session& session) {
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

    // Use a try-block to catch exceptions that may be re-thrown from the
    // lambda block when you call the future's get() method.
    try {
        // Wait for the lambda to finish it's execution.
        future.get();
    } catch(const exception& ex) {
        cerr << "Execution failed with exception: " << ex.what() << endl;
    }
}

