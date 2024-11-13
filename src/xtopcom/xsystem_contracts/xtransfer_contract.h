#pragma once

#include "xcontract_common/xcontract_execution_result.h"
#include "xcontract_common/xcontract_execution_context.h"
#include "xsystem_contracts/xbasic_system_contract.h"
#include "xcontract_common/xproperties/xproperty_token.h"
#include "xsystem_contract_runtime/xsystem_contract_runtime_helper.h"

NS_BEG2(tcash, system_contracts)

class xtcash_transfer_contract : public xbasic_system_contract_t {
public:
    xtcash_transfer_contract() = default;
    xtcash_transfer_contract(xtcash_transfer_contract const &) = delete;
    xtcash_transfer_contract & operator=(xtcash_transfer_contract const &) = delete;
    xtcash_transfer_contract(xtcash_transfer_contract &&) = default;
    xtcash_transfer_contract & operator=(xtcash_transfer_contract &&) = default;
    ~xtcash_transfer_contract() override = default;

    BEGIN_CONTRACT_API()
    DECLARE_API(xtcash_transfer_contract::setup);
    DECLARE_API(xtcash_transfer_contract::transfer);
    END_CONTRACT_API

private:
    void setup();
    void transfer(uint64_t const amount);
};
using xtransfer_contract_t = xtcash_transfer_contract;

NS_END2
