// Copyright (c) 2017-2022 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xaccount_address.h"
#include "xdata/xcons_transaction.h"

namespace tcash {
namespace gasfee {

class xtcash_gas_tx_operator {
public:
    explicit xtcash_gas_tx_operator(xobject_ptr_t<data::xcons_transaction_t> const & tx);
    xtcash_gas_tx_operator(xtcash_gas_tx_operator const &) = delete;
    xtcash_gas_tx_operator & operator=(xtcash_gas_tx_operator const &) = delete;
    xtcash_gas_tx_operator(xtcash_gas_tx_operator &&) = default;
    xtcash_gas_tx_operator & operator=(xtcash_gas_tx_operator &&) = default;
    ~xtcash_gas_tx_operator() = default;

public:
    common::xaccount_address_t sender() const;
    common::xaccount_address_t recver() const;
    std::string sender_str() const;
    std::string recver_str() const;
    data::enum_xtransaction_type tx_type() const;
    base::enum_transaction_subtype tx_subtype() const;
    data::enum_xtransaction_version tx_version() const;
    evm_common::u256 tx_eth_gas_limit() const;
    evm_common::u256 tx_eth_fee_per_gas() const;
    evm_common::u256 tx_eth_limited_gasfee_to_utcash(bool evm_forked = false) const;
    evm_common::u256 tx_eth_priority_fee_per_gas() const;   
    uint64_t deposit() const;
    uint64_t tx_last_action_used_deposit() const;

    evm_common::u256 tx_fixed_tgas() const;
    evm_common::u256 tx_bandwith_tgas() const;
    evm_common::u256 tx_disk_tgas() const;

    static evm_common::u256 balance_to_tgas(const evm_common::u256 balance);
    static evm_common::u256 tgas_to_balance(const evm_common::u256 tgas);
    static evm_common::u256 wei_to_utcash(const evm_common::u256 wei, bool evm_forked);
    static evm_common::u256 utcash_to_wei(const evm_common::u256 utcash);

    bool is_one_stage_tx() const;

    evm_common::u256 get_eth_amount() const;

private:
    xobject_ptr_t<data::xcons_transaction_t> m_tx{nullptr};
};
using xgas_tx_operator_t = xtcash_gas_tx_operator;

}  // namespace gasfee
}  // namespace tcash
