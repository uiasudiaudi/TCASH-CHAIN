// Copyright (c) 2017-2022 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xgasfee/xgas_tx_operator.h"

#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdata/xgenesis_data.h"
#include "xdata/xnative_contract_address.h"
#include "xgasfee/xerror/xerror.h"
#include "xconfig/xutility.h"
#include "xchain_fork/xutility.h"

#include <stdint.h>

namespace tcash {
namespace gasfee {

xtcash_gas_tx_operator::xtcash_gas_tx_operator(xobject_ptr_t<data::xcons_transaction_t> const & tx) : m_tx(tx) {
}

common::xaccount_address_t xtcash_gas_tx_operator::sender() const {
    return common::xaccount_address_t{m_tx->get_source_addr()};
}

common::xaccount_address_t xtcash_gas_tx_operator::recver() const {
    return common::xaccount_address_t{m_tx->get_target_addr()};
}

std::string xtcash_gas_tx_operator::sender_str() const {
    return m_tx->get_source_addr();
}

std::string xtcash_gas_tx_operator::recver_str() const {
    return m_tx->get_target_addr();
}

uint64_t xtcash_gas_tx_operator::deposit() const {
    return m_tx->get_transaction()->get_deposit();
}

data::enum_xtransaction_type xtcash_gas_tx_operator::tx_type() const {
    return static_cast<data::enum_xtransaction_type>(m_tx->get_tx_type());
}

base::enum_transaction_subtype xtcash_gas_tx_operator::tx_subtype() const {
    return m_tx->get_tx_subtype();
}

uint64_t xtcash_gas_tx_operator::tx_last_action_used_deposit() const {
    return m_tx->get_last_action_used_deposit();
}

data::enum_xtransaction_version xtcash_gas_tx_operator::tx_version() const {
    return static_cast<data::enum_xtransaction_version>(m_tx->get_transaction()->get_tx_version());
}

evm_common::u256 xtcash_gas_tx_operator::tx_eth_gas_limit() const {
    return m_tx->get_transaction()->get_gaslimit();
}

evm_common::u256 xtcash_gas_tx_operator::tx_eth_fee_per_gas() const {
    return m_tx->get_transaction()->get_max_fee_per_gas();
}

evm_common::u256 xtcash_gas_tx_operator::tx_eth_priority_fee_per_gas() const {
    return m_tx->get_transaction()->get_max_priority_fee_per_gas();
}

evm_common::u256 xtcash_gas_tx_operator::tx_eth_limited_gasfee_to_utcash(bool evm_forked) const {
    // 1Gwei = (ratio / 10^3)Utcash
    // 1Utcash = (10^3 / ratio)Gwei
    evm_common::u256 limit = tx_eth_gas_limit();
    evm_common::u256 price = tx_eth_fee_per_gas();
    evm_common::u256 wei_gasfee = limit * price;
    evm_common::u256 utcash_gasfee = wei_to_utcash(wei_gasfee, evm_forked);
    xdbg("[xtcash_gas_tx_operator::tx_eth_limited_gasfee_to_utcash] eth_gas_price: %s, eth_gas_limit: %s, wei_gasfee: %s, utcash_gasfee: %s evm_forked %d",
         price.str().c_str(),
         limit.str().c_str(),
         wei_gasfee.str().c_str(),
         utcash_gasfee.str().c_str(),
         evm_forked);
    return utcash_gasfee;
}

evm_common::u256 xtcash_gas_tx_operator::wei_to_utcash(const evm_common::u256 wei, bool evm_forked) {
    evm_common::u256 utcash{0};

    if (evm_forked) {
        // wei -> gwei -> ether -> tcash -> utcash
        // utcash = wei / evm_common::u256(1000000000ULL / evm_common::u256(1000000000ULL) * 1000000ULL * XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio);
        utcash = (wei * XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio)) / evm_common::u256(1000000000000ULL);
    } else {
        evm_common::u256 gwei = wei / evm_common::u256(1000000000ULL);
        evm_common::u256 mtcash = gwei * XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio);
        utcash = mtcash / 1000U;
    }

    xdbg("[xtcash_gas_tx_operator::wei_to_utcash] exchange ratio: %lu, wei: %s, utcash: %s evm_forked %d",
         XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio),
         wei.str().c_str(),
         utcash.str().c_str(), evm_forked);
    return utcash;
}

evm_common::u256 xtcash_gas_tx_operator::utcash_to_wei(const evm_common::u256 utcash) {
    evm_common::u256 wei{0};

    // utcash -> tcash, tcash -> eth, eth -> wei
    // wei = utcash * 1000000ULL / XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio) * 1000000000ULL * 1000000000ULL ;
    wei = (utcash * evm_common::u256(1000000000000ULL)) / XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio);
    xdbg("[xtcash_gas_tx_operator::utcash_to_wei] exchange ratio: %lu, utcash: %s, wei: %s ",
         XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio),
         utcash.str().c_str(),
         wei.str().c_str());
    return wei;
}

evm_common::u256 xtcash_gas_tx_operator::tx_fixed_tgas() const {
    evm_common::u256 fixed_tgas{0};
#ifndef XENABLE_MOCK_ZEC_STAKE
    if (recver_str().empty()) {
        return 0;
    }
    if (!data::is_sys_contract_address(sender()) && data::is_beacon_contract_address(recver())) {
        fixed_tgas = balance_to_tgas(XGET_ONCHAIN_GOVERNANCE_PARAMETER(beacon_tx_fee));
    }
#endif
    return fixed_tgas;
}

evm_common::u256 xtcash_gas_tx_operator::tx_bandwith_tgas() const {
#ifdef ENABLE_SCALE
    uint16_t amplify = 5;
#else
    uint16_t amplify = 1;
#endif
    if (tx_type() != data::xtransaction_type_transfer) {
        amplify = 1;
    }
    evm_common::u256 multiple{3};
    evm_common::u256 bandwith_tgas = multiple * amplify * m_tx->get_transaction()->get_tx_len();
    return bandwith_tgas;
}

evm_common::u256 xtcash_gas_tx_operator::tx_disk_tgas() const {
    if (tx_type() == data::xtransaction_type_transfer) {
        return 0;
    }
    evm_common::u256 multiple{1};
    // evm deploy tx
    if (recver_str().empty() || recver() == eth_zero_address) {
        multiple = 1200000UL;
    }
    evm_common::u256 disk_tgas = multiple * m_tx->get_transaction()->get_tx_len();
    return disk_tgas;
}

bool xtcash_gas_tx_operator::is_one_stage_tx() const {
    return (m_tx->is_self_tx() || m_tx->get_inner_table_flag());
}

evm_common::u256 xtcash_gas_tx_operator::balance_to_tgas(const evm_common::u256 balance) {
    xassert(XGET_ONCHAIN_GOVERNANCE_PARAMETER(tx_deposit_gas_exchange_ratio) > 0);
    return balance / XGET_ONCHAIN_GOVERNANCE_PARAMETER(tx_deposit_gas_exchange_ratio);
}

evm_common::u256 xtcash_gas_tx_operator::tgas_to_balance(const evm_common::u256 tgas) {
    return tgas * XGET_ONCHAIN_GOVERNANCE_PARAMETER(tx_deposit_gas_exchange_ratio);
}

evm_common::u256 xtcash_gas_tx_operator::get_eth_amount() const {
    return (m_tx->get_transaction()->get_amount_256());
}

}  // namespace gasfee
}  // namespace tcash
