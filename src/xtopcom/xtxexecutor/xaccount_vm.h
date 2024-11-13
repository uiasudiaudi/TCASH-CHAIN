﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <vector>

#include "xdata/xcons_transaction.h"
#include "xstore/xaccount_context.h"
#include "xtxexecutor/xvm_face.h"

#include "xcontract_runtime/xtransaction_execution_result.h"

NS_BEG2(tcash, txexecutor)

class xaccount_vm_t {
 public:
    xaccount_vm_t(const data::xaccountstate_ptr_t & accountstate, const statectx::xstatectx_face_ptr_t & statectx);

 public:
    int32_t execute(const xvm_input_t & input, xvm_output_t & output);
    uint64_t get_total_gas_burn() const;
 
 private:
    contract_runtime::xtransaction_execution_result_t exec_evm_tx(store::xaccount_context_t * account_context, const xcons_transaction_ptr_t & tx);
    int32_t exec_one_tx(store::xaccount_context_t * account_context, const xcons_transaction_ptr_t & tx);
    int32_t exec_tx(store::xaccount_context_t * account_context, const xcons_transaction_ptr_t & tx, std::vector<xcons_transaction_ptr_t> & contract_create_txs);

 private:
    store::xaccount_context_ptr_t m_account_context{nullptr};
};

NS_END2
