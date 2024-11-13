#pragma once

#include "xbasic/xmemory.hpp"
#include "xcontract_common/xbasic_contract.h"

NS_BEG2(tcash, system_contracts)

class xtcash_basic_system_contract : public contract_common::xbasic_contract_t {
public:
    xtcash_basic_system_contract(xtcash_basic_system_contract const &) = delete;
    xtcash_basic_system_contract & operator=(xtcash_basic_system_contract const &) = delete;
    xtcash_basic_system_contract(xtcash_basic_system_contract &&) = default;
    xtcash_basic_system_contract & operator=(xtcash_basic_system_contract &&) = default;
    ~xtcash_basic_system_contract() override = default;

protected:
    xtcash_basic_system_contract() = default;
};
using xbasic_system_contract_t = xtcash_basic_system_contract;

NS_END2
