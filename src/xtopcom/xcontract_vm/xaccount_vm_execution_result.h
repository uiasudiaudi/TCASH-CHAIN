// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcontract_runtime/xtransaction_execution_result.h"
#include "xcontract_vm/xerror/xerror.h"

#include <string>
#include <system_error>
#include <vector>

NS_BEG2(tcash, contract_vm)

struct xtcash_account_vm_execution_status {
    std::error_code ec{error::xerrc_t::ok};
    std::string extra_msg{};
};
using xaccount_vm_execution_status_t = xtcash_account_vm_execution_status;

struct xtcash_account_vm_execution_result {
    xaccount_vm_execution_status_t status;
    std::string binlog;
    std::string bincode;
    std::vector<contract_runtime::xtransaction_execution_result_t> transaction_results;
};
using xaccount_vm_execution_result_t = xtcash_account_vm_execution_result;

struct xtcash_account_vm_output {
    xaccount_vm_execution_status_t status;
    std::string binlog;
    std::string bincode;
    std::vector<data::xcons_transaction_ptr_t> success_tx_assemble;
    std::vector<data::xcons_transaction_ptr_t> failed_tx_assemble;
    std::vector<data::xcons_transaction_ptr_t> delay_tx_assemble;
};
using xaccount_vm_output_t = xtcash_account_vm_output;

NS_END2
