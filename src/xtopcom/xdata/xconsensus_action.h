// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xobject_ptr.h"
#include "xbasic/xbyte_buffer.h"
#include "xcommon/xaddress.h"
#include "xdata/xaction.h"
#include "xdata/xcons_transaction.h"
#include "xdata/xconsensus_action_fwd.h"
#include "xdata/xconsensus_action_stage.h"
#include "xdata/xnative_contract_address.h"
#include "xdata/xreceipt_data_store.h"
#include "xdata/xtcash_action.h"

#include <cstdint>
#include <string>

NS_BEG2(tcash, data)

using xaction_consensus_exec_status = data::enum_xunit_tx_exec_status;
template <xtcash_action_type_t ActionTypeV>
class xtcash_consensus_action : public xtcash_action_t<ActionTypeV> {
public:
    xtcash_consensus_action(xtcash_consensus_action const &) = default;
    xtcash_consensus_action & operator=(xtcash_consensus_action const &) = default;
    xtcash_consensus_action(xtcash_consensus_action &&) = default;
    xtcash_consensus_action & operator=(xtcash_consensus_action &&) = default;
    ~xtcash_consensus_action() override = default;

    explicit xtcash_consensus_action(xobject_ptr_t<data::xcons_transaction_t> const & tx) noexcept;

    xconsensus_action_stage_t stage() const noexcept;
    common::xaccount_address_t sender() const;
    common::xaccount_address_t recver() const;
    common::xaccount_address_t contract_address() const;
    common::xaccount_address_t execution_address() const;
    uint64_t max_gas_amount() const;
    uint64_t last_nonce() const noexcept;
    uint64_t nonce() const noexcept;
    uint256_t hash() const noexcept;
    std::string source_action_name() const;
    std::string target_action_name() const;
    xbyte_buffer_t source_action_data() const;
    xbyte_buffer_t target_action_data() const;
    xreceipt_data_store_t receipt_data() const;
    xaction_consensus_exec_status action_consensus_result() const;
    data::enum_xtransaction_type transaction_type() const;
    data::enum_xaction_type source_action_type() const;
    data::enum_xaction_type target_action_type() const;
    uint32_t size() const;
    uint64_t deposit() const;
    std::string digest_hex() const;
    uint32_t used_tgas() const;
    uint32_t used_disk() const;
    uint32_t last_action_send_tx_lock_tgas() const;
    uint32_t last_action_used_deposit() const;
    uint32_t last_action_recv_tx_use_send_tx_tgas() const;
    enum_xunit_tx_exec_status last_action_exec_status() const;
};

NS_END2

#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xvledger/xvblock.h"

NS_BEG2(tcash, data)

template <xtcash_action_type_t ActionTypeV>
xtcash_consensus_action<ActionTypeV>::xtcash_consensus_action(xobject_ptr_t<data::xcons_transaction_t> const & tx) noexcept : xtcash_tcash_action<ActionTypeV>{ tx, tx->is_send_tx() ? static_cast<common::xlogic_time_t>((tx->get_transaction()->get_fire_timestamp() + tx->get_transaction()->get_expire_duration() + XGET_ONCHAIN_GOVERNANCE_PARAMETER(tx_send_timestamp_tolerance)) / XGLOBAL_TIMER_INTERVAL_IN_SECONDS) : common::xjudgement_day } {
}

template <xtcash_action_type_t ActionTypeV>
xconsensus_action_stage_t xtcash_consensus_action<ActionTypeV>::stage() const noexcept {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    switch (tx->get_tx_subtype()) {  // NOLINT(clang-diagnostic-switch-enum)
    case base::enum_transaction_subtype_send:
        return xconsensus_action_stage_t::send;

    case base::enum_transaction_subtype_recv:
        return xconsensus_action_stage_t::recv;

    case base::enum_transaction_subtype_confirm:
        return xconsensus_action_stage_t::confirm;

    case base::enum_transaction_subtype_self:
        return xconsensus_action_stage_t::self;

    default:
        assert(false);
        return xconsensus_action_stage_t::invalid;
    }
}

template <xtcash_action_type_t ActionTypeV>
common::xaccount_address_t xtcash_consensus_action<ActionTypeV>::sender() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    return common::xaccount_address_t{ tx->get_source_addr() };
}

template <xtcash_action_type_t ActionTypeV>
common::xaccount_address_t xtcash_consensus_action<ActionTypeV>::recver() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    return common::xaccount_address_t{ tx->get_target_addr() };
}

template <xtcash_action_type_t ActionTypeV>
common::xaccount_address_t xtcash_consensus_action<ActionTypeV>::contract_address() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    return common::xaccount_address_t{ tx->get_target_addr() };
}

template <xtcash_action_type_t ActionTypeV>
common::xaccount_address_t xtcash_consensus_action<ActionTypeV>::execution_address() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    switch (stage()) {
    case xconsensus_action_stage_t::send:
        XATTRIBUTE_FALLTHROUGH;
    case xconsensus_action_stage_t::confirm:
        return common::xaccount_address_t{ tx->get_source_addr() };

    case xconsensus_action_stage_t::recv:
        return common::xaccount_address_t{ tx->get_target_addr() };

    case xconsensus_action_stage_t::self:
        return common::xaccount_address_t{ tx->get_target_addr() };

    default:
        assert(false);
        return common::xaccount_address_t{};
    }
}

template <xtcash_action_type_t ActionTypeV>
uint64_t xtcash_consensus_action<ActionTypeV>::max_gas_amount() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    return XGET_ONCHAIN_GOVERNANCE_PARAMETER(tx_deposit_gas_exchange_ratio) * tx->get_transaction()->get_deposit(); // TODO free tgas is missing here.
}

template <xtcash_action_type_t ActionTypeV>
uint64_t xtcash_consensus_action<ActionTypeV>::nonce() const noexcept {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_tx_nonce();
}

template <xtcash_action_type_t ActionTypeV>
uint64_t xtcash_consensus_action<ActionTypeV>::last_nonce() const noexcept {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_last_nonce();
}

template <xtcash_action_type_t ActionTypeV>
uint256_t xtcash_consensus_action<ActionTypeV>::hash() const noexcept {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_tx_hash_256();
}

template <xtcash_action_type_t ActionTypeV>
std::string xtcash_consensus_action<ActionTypeV>::source_action_name() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_source_action_name();
}

template <xtcash_action_type_t ActionTypeV>
std::string xtcash_consensus_action<ActionTypeV>::target_action_name() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_target_action_name();
}

template <xtcash_action_type_t ActionTypeV>
xbyte_buffer_t xtcash_consensus_action<ActionTypeV>::source_action_data() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    auto const & transaction = tx->get_transaction();
    auto const & action_param = transaction->get_source_action_para();
    xdbg("action param size %zu", action_param.size());

    return {std::begin(action_param), std::end(action_param)};
}

template <xtcash_action_type_t ActionTypeV>
xbyte_buffer_t xtcash_consensus_action<ActionTypeV>::target_action_data() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    auto const & transaction = tx->get_transaction();
    auto const & action_param = transaction->get_target_action_para();

    return { std::begin(action_param), std::end(action_param) };
}

template <xtcash_action_type_t ActionTypeV>
xreceipt_data_store_t xtcash_consensus_action<ActionTypeV>::receipt_data() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    return tx->get_last_action_receipt_data();
}

template <xtcash_action_type_t ActionTypeV>
xaction_consensus_exec_status xtcash_consensus_action<ActionTypeV>::action_consensus_result() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);
    return tx->get_last_action_exec_status();
}

template <xtcash_action_type_t ActionTypeV>
data::enum_xtransaction_type xtcash_consensus_action<ActionTypeV>::transaction_type() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return data::enum_xtransaction_type(tx->get_tx_type());
}

template <xtcash_action_type_t ActionTypeV>
data::enum_xaction_type xtcash_consensus_action<ActionTypeV>::source_action_type() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_source_action_type();
}

template <xtcash_action_type_t ActionTypeV>
data::enum_xaction_type xtcash_consensus_action<ActionTypeV>::target_action_type() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_target_action_type();
}

template <xtcash_action_type_t ActionTypeV>
uint32_t xtcash_consensus_action<ActionTypeV>::size() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_tx_len();
}

template <xtcash_action_type_t ActionTypeV>
uint64_t xtcash_consensus_action<ActionTypeV>::deposit() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_transaction()->get_deposit();
}

template <xtcash_action_type_t ActionTypeV>
std::string xtcash_consensus_action<ActionTypeV>::digest_hex() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_digest_hex_str();
}

template <xtcash_action_type_t ActionTypeV>
uint32_t xtcash_consensus_action<ActionTypeV>::used_tgas() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_current_used_tgas();
}

template <xtcash_action_type_t ActionTypeV>
uint32_t xtcash_consensus_action<ActionTypeV>::used_disk() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_current_used_disk();
}

template <xtcash_action_type_t ActionTypeV>
uint32_t xtcash_consensus_action<ActionTypeV>::last_action_send_tx_lock_tgas() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_last_action_send_tx_lock_tgas();
}

template <xtcash_action_type_t ActionTypeV>
uint32_t xtcash_consensus_action<ActionTypeV>::last_action_used_deposit() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_last_action_used_deposit();
}

template <xtcash_action_type_t ActionTypeV>
uint32_t xtcash_consensus_action<ActionTypeV>::last_action_recv_tx_use_send_tx_tgas() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_last_action_recv_tx_use_send_tx_tgas();
}

template <xtcash_action_type_t ActionTypeV>
enum_xunit_tx_exec_status xtcash_consensus_action<ActionTypeV>::last_action_exec_status() const {
    auto const & tx = dynamic_xobject_ptr_cast<data::xcons_transaction_t>(this->m_action_src);
    assert(tx != nullptr);

    return tx->get_last_action_exec_status();
}


/// evm_action:

enum xtcash_evm_action_type {
    invalid = 0,
    deploy_contract = 1,
    call_contract = 2,
};
using xevm_action_type_t = xtcash_evm_action_type;

template <>
class xtcash_consensus_action<xtcash_action_type_t::evm> : public xtcash_action_t<xtcash_action_type_t::evm> {
private:
    common::xaccount_address_t m_sender;
    common::xaccount_address_t m_recver;
    evm_common::u256 m_value;
    xbytes_t m_input_data;
    xevm_action_type_t m_evm_action_type{xtcash_evm_action_type::invalid};
    uint64_t m_gaslimit;

public:
    xtcash_consensus_action(xtcash_consensus_action const &) = default;
    xtcash_consensus_action & operator=(xtcash_consensus_action const &) = default;
    xtcash_consensus_action(xtcash_consensus_action &&) = default;
    xtcash_consensus_action & operator=(xtcash_consensus_action &&) = default;
    ~xtcash_consensus_action() override = default;

    xtcash_consensus_action(common::xaccount_address_t src_address, common::xaccount_address_t dst_address, evm_common::u256 value, xbytes_t data, uint64_t gaslimit);
    explicit xtcash_consensus_action(xobject_ptr_t<data::xcons_transaction_t> const & tx);

    xevm_action_type_t evm_action_type() const noexcept;
    common::xaccount_address_t const & sender() const noexcept;
    common::xaccount_address_t const & recver() const noexcept;
    xbytes_t const & data() const noexcept;
    evm_common::u256 const & value() const noexcept;
    uint64_t gas_limit() const noexcept;
};

NS_END2
