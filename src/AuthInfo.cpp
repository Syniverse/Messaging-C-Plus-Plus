#include <iostream>
#include <cstdio>
#include <boost/filesystem.hpp>
#include "restc-cpp/logging.h"
#include "restc-cpp/restc-cpp.h"
#include "restc-cpp/SerializeJson.h"
#include "scgapi/AuthInfo.h"

using namespace std;
using namespace restc_cpp;

namespace scg_api {

AuthInfo::AuthInfo(const boost::filesystem::path& jsonConfig) {
    if (!boost::filesystem::is_regular(jsonConfig)) {
        throw runtime_error(string("Failed to open configuration file: ") 
            + jsonConfig.native());
    }
    
    ifstream ifs(jsonConfig.native());
    SerializeFromJson(config_, ifs);
}
    
} //namespace
