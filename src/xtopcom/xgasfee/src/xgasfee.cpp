// Copyright (c) 2017-2022 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xgasfee/xgasfee.h"

#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdata/xgenesis_data.h"
#include "xgasfee/xerror/xerror.h"

#define CHECK_EC_RETURN(ec)                                                                                                                                                        \
    do {                                                                                                                                                                           \
        if (ec) {                                                                                                                                                                  \
            xwarn("[xtcash_gasfee] error occured, category: %s, msg: %s!", ec.category().name(), ec.message().c_str());                                                        \
            return;                                                                                                                                                                \
        }                                                                                                                                                                          \
    } while (0)

namespace tcash {
namespace gasfee {

xtcash_gasfee::xtcash_gasfee(std::shared_ptr<data::xunit_bstate_t> const & state, xobject_ptr_t<data::xcons_transaction_t> const & tx, uint64_t time, uint64_t onchain_tgas_deposit)
  : xgas_state_operator_t(state), xgas_tx_operator_t(tx), m_time(time), m_onchain_tgas_deposit(onchain_tgas_deposit) {
    xassert(state != nullptr);
    xassert(tx != nullptr);
}

void xtcash_gasfee::check(std::error_code & ec) {
    if (deposit() > 0) {
        xassert(false);
        xerror("[xtcash_gasfee::check] unreachable code, deposit: %lu", deposit());
        ec = gasfee::error::xenum_errc::invalid_param;
        return;
    }
    // tcash balance
    const uint64_t balance = account_balance();
    // gas fee
    const evm_common::u256 eth_max_gasfee = tx_eth_limited_gasfee_to_utcash();
    if (eth_max_gasfee > balance) {
        // tcash balance not enough
        ec = gasfee::error::xenum_errc::account_balance_not_enough;
        xwarn("[xtcash_gasfee::check] account_balance_not_enough, eth_max_gasfee: %s, balance: %lu", eth_max_gasfee.str().c_str(), balance);
        return;
    }
    if (eth_max_gasfee == 0) {
        ec = gasfee::error::xenum_errc::invalid_param;
        xwarn("[xtcash_gasfee::check] invalid gasfee: %s", eth_max_gasfee.str().c_str());
        return;
    }
    xdbg("[xtcash_gasfee::check] valid_tx, eth_max_gasfee: %s, balance: %lu", eth_max_gasfee.str().c_str(), balance);
    return;
}

void xtcash_gasfee::init(std::error_code & ec) {
    // tcash balance
    const uint64_t balance = account_balance();
    // gas fee
    const evm_common::u256 eth_max_gasfee = tx_eth_limited_gasfee_to_utcash();
    xdbg("[xtcash_gasfee::init] evm tx, eth_max_gasfee: %s", eth_max_gasfee.str().c_str());
    if (eth_max_gasfee > balance) {
        // tcash balance not enough
        ec = gasfee::error::xenum_errc::account_balance_not_enough;
        xwarn("[xtcash_gasfee::init] account_balance_not_enough, eth_max_gasfee: %s, balance: %lu", eth_max_gasfee.str().c_str(), balance);
        return;
    }
    m_converted_tgas = balance_to_tgas(eth_max_gasfee);
    m_free_tgas = account_available_tgas(m_time, m_onchain_tgas_deposit);
    xdbg("[xtcash_gasfee::init] final, eth_max_gasfee: %s, balance: %lu, m_free_tgas: %s, m_converted_tgas: %s", eth_max_gasfee.str().c_str(), balance, m_free_tgas.str().c_str(), m_converted_tgas.str().c_str());
    return;
}

void xtcash_gasfee::add(const evm_common::u256 tgas, std::error_code & ec) {
    xassert(m_free_tgas >= m_free_tgas_usage);
    xassert(m_converted_tgas >= m_converted_tgas_usage);
    const evm_common::u256 left_available_tgas = m_free_tgas + m_converted_tgas - m_free_tgas_usage - m_converted_tgas_usage;
    xdbg("[xtcash_gasfee::add] supplement: %s, m_free_tgas_usage: %s, m_converted_tgas_usage: %s, m_free_tgas: %s, m_converted_tgas: %s, left_available_tgas: %s",
         tgas.str().c_str(),
         m_free_tgas_usage.str().c_str(),
         m_converted_tgas_usage.str().c_str(),
         m_free_tgas.str().c_str(),
         m_converted_tgas.str().c_str(),
         left_available_tgas.str().c_str());
    if (tgas == 0) {
        return;
    }
    if (tgas > left_available_tgas) {
        m_converted_tgas_usage = m_converted_tgas;
        m_free_tgas_usage = m_free_tgas;
        xwarn("[xtcash_gasfee::add] transaction not enough deposit to tgas, m_converted_tgas_usage to: %s, m_free_tgas_usage to: %s",
              m_converted_tgas_usage.str().c_str(),
              m_free_tgas_usage.str().c_str());
        ec = gasfee::error::xenum_errc::tx_out_of_gas;
    } else if (tgas > (m_free_tgas - m_free_tgas_usage)) {
        m_converted_tgas_usage += tgas - (m_free_tgas - m_free_tgas_usage);
        m_free_tgas_usage = m_free_tgas;
        xdbg("[xtcash_gasfee::add] m_converted_tgas_usage to: %s, m_free_tgas_usage to: %s", m_converted_tgas_usage.str().c_str(), m_free_tgas_usage.str().c_str());
    } else {
        m_free_tgas_usage += tgas;
        xdbg("[xtcash_gasfee::add] m_free_tgas_usage to: %s", m_free_tgas_usage.str().c_str());
    }
    return;
}

void xtcash_gasfee::calculate(const evm_common::u256 supplement_gas, std::error_code & ec) {
    // 1. fixed tgas
    process_fixed_tgas(ec);
    CHECK_EC_RETURN(ec);
    // 2. bandwith tgas
    process_bandwith_tgas(ec);
    CHECK_EC_RETURN(ec);
    // 3. disk tgas
    process_disk_tgas(ec);
    CHECK_EC_RETURN(ec);
    // 4. calculation tgas
    process_calculation_tgas(supplement_gas, ec);
    CHECK_EC_RETURN(ec);
}

void xtcash_gasfee::process_fixed_tgas(std::error_code & ec) {
    evm_common::u256 fixed_tgas = tx_fixed_tgas();
    xdbg("[xtcash_gasfee::process_fixed_tgas] fixed_tgas to burn: %s", fixed_tgas.str().c_str());
    add(fixed_tgas, ec);
    CHECK_EC_RETURN(ec);
}

void xtcash_gasfee::process_bandwith_tgas(std::error_code & ec) {
    evm_common::u256 bandwith_tgas = tx_bandwith_tgas();
    xdbg("[xtcash_gasfee::process_bandwith_tgas] bandwith_tgas: %s", bandwith_tgas.str().c_str());
    add(bandwith_tgas, ec);
    CHECK_EC_RETURN(ec);
}

void xtcash_gasfee::process_disk_tgas(std::error_code & ec) {
    evm_common::u256 disk_tgas = tx_disk_tgas();
    xdbg("[xtcash_gasfee::process_disk_tgas] disk_tgas_: %s", disk_tgas.str().c_str());
    add(disk_tgas, ec);
    CHECK_EC_RETURN(ec);
}

void xtcash_gasfee::process_calculation_tgas(const evm_common::u256 calculation_gas, std::error_code & ec) {
    xdbg("[xtcash_gasfee::process_calculation_tgas] calculation_gas: %s", calculation_gas.str().c_str());
    evm_common::u256 gas_limit = tx_eth_gas_limit();
    if (calculation_gas > gas_limit) {
        xwarn("[xtcash_gasfee::process_calculation_tgas] calculation_gas %s over limit %s", calculation_gas.str().c_str(), gas_limit.str().c_str());
        ec = gasfee::error::xenum_errc::tx_out_of_gas;
        m_converted_tgas_usage = m_converted_tgas;
        m_free_tgas_usage = m_free_tgas;
        xwarn("[xtcash_gasfee::process_calculation_tgas] transaction out of gas, m_converted_tgas_usage to: %s, m_free_tgas_usage to: %s",
              m_converted_tgas_usage.str().c_str(),
              m_free_tgas_usage.str().c_str());
        return;
    }
    evm_common::u256 calculation_tgas = calculation_gas * XGET_ONCHAIN_GOVERNANCE_PARAMETER(eth_gas_to_tgas_exchange_ratio);
    add(calculation_tgas, ec);
    CHECK_EC_RETURN(ec);
}

void xtcash_gasfee::store_in_one_stage() {
    evm_common::u256 state_used_tgas = m_free_tgas_usage + account_formular_used_tgas(m_time);
    if (state_used_tgas > UINT64_MAX) {
        xwarn("[xtcash_gasfee::store_in_one_stage] state_used_tgas %s over linit", state_used_tgas.str().c_str());
        state_used_tgas = UINT64_MAX;
    }
    m_detail.m_state_used_tgas = static_cast<uint64_t>(state_used_tgas);
    m_detail.m_state_last_time = m_time;
    evm_common::u256 tcash_usage = tgas_to_balance(m_converted_tgas_usage);
    if (tcash_usage > UINT64_MAX) {
        xwarn("[xtcash_gasfee::store_in_one_stage] tcash_usage %s over linit", tcash_usage.str().c_str());
        tcash_usage = UINT64_MAX;
    }
    m_detail.m_state_burn_balance = static_cast<uint64_t>(tcash_usage);
    evm_common::u256 tx_used_tgas = m_free_tgas_usage;
    if (tx_used_tgas > UINT64_MAX) {
        xwarn("[xtcash_gasfee::store_in_one_stage] tx_used_tgas %s over linit", tx_used_tgas.str().c_str());
        tx_used_tgas = UINT64_MAX;
    }
    m_detail.m_tx_used_tgas = static_cast<uint64_t>(tx_used_tgas);
    evm_common::u256 tx_used_deposit = tcash_usage;
    if (tx_used_deposit > UINT64_MAX) {
        xwarn("[xtcash_gasfee::store_in_one_stage] tx_used_deposit %s over linit", tx_used_deposit.str().c_str());
        tx_used_deposit = UINT64_MAX;
    }
    m_detail.m_tx_used_deposit = static_cast<uint64_t>(tx_used_deposit);
    xdbg("[xtcash_gasfee::store_in_one_stage] m_free_tgas: %s, m_converted_tgas_usage: %s, tcash_usage: %s",
         m_free_tgas.str().c_str(),
         m_converted_tgas_usage.str().c_str(),
         tcash_usage.str().c_str());
    xdbg("[xtcash_gasfee::store_in_one_stage] gasfee_detail: %s", m_detail.str().c_str());
}


void xtcash_gasfee::postprocess_one_stage(const evm_common::u256 supplement_gas, std::error_code & ec) {
    do {
        init(ec);
        if (ec) {
            break;
        }
        calculate(supplement_gas, ec);
        if (ec) {
            break;
        }
    } while(0);
    store_in_one_stage();
}



void xtcash_gasfee::preprocess(std::error_code & ec) {
    // ignore gas calculation of system contracts
    if (data::is_sys_contract_address(sender())) {
        return;
    }
    if (is_one_stage_tx()) {
        // preprocess_one_stage(ec);
        check(ec);
        CHECK_EC_RETURN(ec);
    } else {
        // unreachable code
        xassert(false);
        xerror("[xtcash_gasfee::preprocess] unreachable code");
        ec = gasfee::error::xenum_errc::invalid_param;
    }
}

void xtcash_gasfee::postprocess(const evm_common::u256 supplement_gas, std::error_code & ec) {
     // ignore gas calculation of system contracts
     if (data::is_sys_contract_address(sender())) {
         return;
     }
    if (is_one_stage_tx()) {
        postprocess_one_stage(supplement_gas, ec);
    } else {
        // unreachable code
        xassert(false);
        xerror("[xtcash_gasfee::postprocess] unreachable code");
        ec = gasfee::error::xenum_errc::invalid_param;
    }
    CHECK_EC_RETURN(ec);
}

txexecutor::xvm_gasfee_detail_t xtcash_gasfee::gasfee_detail() const {
    return m_detail;
}

}  // namespace gasfee
}  // namespace tcash