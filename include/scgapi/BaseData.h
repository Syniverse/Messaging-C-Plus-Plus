#pragma once

#include <vector>
#include <memory>
#include <map>

#include <boost/utility/identity_type.hpp>

#include "scgapi/Scg.h"
#include "scgapi/Session.h"
#include "scgapi/AsyncForwardList.h"
#include "scgapi/ResourceImpl.h"

namespace scg_api {

/*! \class BaseData BaseData.h "scg_api/BaseData.h"
 *
 * Abstract base class for SCG data objecs.
 *
 * This is for convenience in case you want to create generic
 * containers for any SCG data type.
 */

class BaseData
{
public:
    virtual ~BaseData() = default;


protected:
    void VerifyForOperations_(const std::string& id) {
        if (id.empty()) {
            throw std::runtime_error(
                "This object is not initialized for operations");
        }
        if (res_exists_.expired()) {
            throw std::runtime_error(
                "This object's resource has expired");
        }
    }

    std::weak_ptr<int> res_exists_;
};

} // namespace

