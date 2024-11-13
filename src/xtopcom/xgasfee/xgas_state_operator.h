// Copyright (c) 2017-2022 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xvmethod.h"
#include "xdata/xunit_bstate.h"
#include "xstate_accessor/xstate_accessor.h"

namespace tcash {
namespace gasfee {

class xtcash_gas_state_operator {
public:
    explicit xtcash_gas_state_operator(std::shared_ptr<data::xunit_bstate_t> const & state);
    xtcash_gas_state_operator(xtcash_gas_state_operator const &) = delete;
    xtcash_gas_state_operator & operator=(xtcash_gas_state_operator const &) = delete;
    xtcash_gas_state_operator(xtcash_gas_state_operator &&) = default;
    xtcash_gas_state_operator & operator=(xtcash_gas_state_operator &&) = default;
    ~xtcash_gas_state_operator() = default;

    uint64_t account_balance() const;
    evm_common::u256 account_eth_balance() const;
    uint64_t account_available_tgas(uint64_t current_time, uint64_t onchain_total_gas_deposit) const;
    uint64_t account_formular_used_tgas(uint64_t current_time) const;

private:
    std::shared_ptr<data::xunit_bstate_t> m_state{nullptr};
};
using xgas_state_operator_t = xtcash_gas_state_operator;

}  // namespace gasfee
}  // namespace tcash
