#pragma once

#include "xcontract_common/xbasic_stateless_contract.h"
#include "xcontract_common/xcontract_execution_context.h"
#include "xcontract_common/xcontract_execution_result.h"
#include "xtxexecutor/xcontract/xcontract_runtime_helper.h"

NS_BEG3(tcash, txexecutor, contract)

class xtcash_transfer_contract : public contract_common::xbasic_stateless_contract_t {
public:
    xtcash_transfer_contract() = default;
    xtcash_transfer_contract(xtcash_transfer_contract const &) = delete;
    xtcash_transfer_contract & operator=(xtcash_transfer_contract const &) = delete;
    xtcash_transfer_contract(xtcash_transfer_contract &&) = default;
    xtcash_transfer_contract & operator=(xtcash_transfer_contract &&) = default;
    ~xtcash_transfer_contract() override = default;

    BEGIN_CONTRACT_API()
    DECLARE_API(xtcash_transfer_contract::transfer);
    DECLARE_API(xtcash_transfer_contract::deposit);
    DECLARE_API(xtcash_transfer_contract::withdraw);
    END_CONTRACT_API

private:
    void transfer(const std::string & token_name, const std::string & amount_256_str);
    void deposit(const std::string & token_name, const std::string & amount_256_str);
    void withdraw(const std::string & token_name, const std::string & amount_256_str);
};
using xtransfer_contract_t = xtcash_transfer_contract;

NS_END3
