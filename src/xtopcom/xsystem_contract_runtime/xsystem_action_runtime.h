// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xsystem_contract_runtime/xsystem_contract_manager.h"
#include "xbasic/xmemory.hpp"
#include "xcontract_runtime/xaction_runtime.h"
#include "xdata/xconsensus_action.h"

NS_BEG2(tcash, contract_runtime)

template <>
class xtcash_action_runtime<data::xsystem_consensus_action_t> {
private:
    observer_ptr<system::xsystem_contract_manager_t> system_contract_manager_;

public:
    // xtcash_action_runtime() = default;
    xtcash_action_runtime(xtcash_action_runtime const &) = delete;
    xtcash_action_runtime & operator=(xtcash_action_runtime const &) = delete;
    xtcash_action_runtime(xtcash_action_runtime &&) = default;
    xtcash_action_runtime & operator=(xtcash_action_runtime &&) = default;
    ~xtcash_action_runtime() = default;

    explicit xtcash_action_runtime(observer_ptr<system::xsystem_contract_manager_t> const & system_contract_manager) noexcept;

    std::unique_ptr<xaction_session_t<data::xsystem_consensus_action_t>> new_session(observer_ptr<contract_common::xcontract_state_t> contract_state);

    xtransaction_execution_result_t execute(observer_ptr<contract_common::xcontract_execution_context_t> tx_ctx);
};

NS_END2

NS_BEG3(tcash, contract_runtime, system)
using xsystem_action_runtime_t = xaction_runtime_t<data::xsystem_consensus_action_t>;
NS_END3
