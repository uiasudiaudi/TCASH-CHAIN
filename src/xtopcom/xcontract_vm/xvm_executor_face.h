// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpedantic"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
#    pragma warning(push, 0)
#endif

#include "xvledger/xvstate.h"

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include "xbase/xobject_ptr.h"
#include "xcontract_vm/xaccount_vm_execution_result.h"
#include "xdata/xcons_transaction.h"
#include "xdata/xtcash_action.h"

NS_BEG2(tcash, contract_vm)

class xtcash_vm_executor_face {
public:
    xtcash_vm_executor_face() = default;
    xtcash_vm_executor_face(xtcash_vm_executor_face const &) = delete;
    xtcash_vm_executor_face & operator=(xtcash_vm_executor_face const &) = delete;
    xtcash_vm_executor_face(xtcash_vm_executor_face &&) = default;
    xtcash_vm_executor_face & operator=(xtcash_vm_executor_face &&) = default;
    virtual ~xtcash_vm_executor_face() = default;

    virtual xaccount_vm_output_t execute(std::vector<data::xcons_transaction_ptr_t> const & txs,
                                         observer_ptr<base::xvbstate_t> const & state,
                                         data::xblock_consensus_para_t const & cs_para) = 0;
};
using xvm_executor_face_t = xtcash_vm_executor_face;

NS_END2
