#include "xcontract_common/xproperties/xproperty_access_control.h"

using namespace tcash::contract_common::properties;

class xtcash_property_api: public xproperty_access_control_t {
public:
    xtcash_property_api(tcash::observer_ptr<tcash::base::xvbstate_t> bstate, xproperty_access_control_data_t ac_data): xproperty_access_control_t(bstate, ac_data){}
    ~xtcash_property_api() = default;

    bool read_permitted(tcash::common::xaccount_address_t const & reader, xproperty_identifier_t const & property_id)  const noexcept override final {
        return true;
    }

    bool write_permitted(tcash::common::xaccount_address_t const & writer, xproperty_identifier_t const & property_id) const noexcept override final {
        return true;
    }
};
using xproperty_api_t = xtcash_property_api;