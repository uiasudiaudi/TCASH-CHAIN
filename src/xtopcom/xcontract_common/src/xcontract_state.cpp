// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xcontract_common/xcontract_state.h"

#include "xbasic/xerror/xerror.h"
#include "xbasic/xutility.h"

#include <cassert>
#include <cinttypes>
#include <iomanip>

NS_BEG2(tcash, contract_common)

xtcash_contract_state::xtcash_contract_state(common::xaccount_address_t action_account_addr,
                                         observer_ptr<state_accessor::xstate_accessor_t> const sa,
                                         xcontract_execution_param_t const & execution_param)
  : m_action_account_address{std::move(action_account_addr)}, m_state_accessor{sa}, m_param{execution_param} {
    m_latest_followup_tx_hash = latest_sendtx_hash();
    m_latest_followup_tx_nonce = latest_sendtx_nonce();
}

xtcash_contract_state::xtcash_contract_state(common::xaccount_address_t action_account_addr, statectx::xstatectx_face_ptr_t const state_ctx)
  : m_action_account_address{std::move(action_account_addr)}, m_state_ctx{state_ctx} {
    xassert(m_state_ctx != nullptr);
}

xtcash_contract_state::xtcash_contract_state(common::xaccount_address_t const & account_address)
  : m_action_account_address{account_address}
  , m_state_accessor_owned{state_accessor::xstate_accessor_t::build_from(account_address)}
  , m_state_accessor{make_observer(m_state_accessor_owned.get())} {
}

xtcash_contract_state::xtcash_contract_state(common::xaccount_address_t const & account_address, uint64_t const height)
  : m_action_account_address{account_address}
  , m_state_accessor_owned{state_accessor::xstate_accessor_t::build_from(account_address, height)}
  , m_state_accessor{make_observer(m_state_accessor_owned.get())} {
}

std::unique_ptr<xtcash_contract_state> xtcash_contract_state::build_from(common::xaccount_address_t const & address) {
    auto * contract_state = new xtcash_contract_state{address};
    return std::unique_ptr<xtcash_contract_state>{contract_state};
}

std::unique_ptr<xtcash_contract_state> xtcash_contract_state::build_from(common::xaccount_address_t const & address, std::error_code & ec) {
    assert(!ec);

    try {
        return build_from(address);
    } catch (tcash::error::xtcash_error_t const & eh) {
        ec = eh.code();
    }

    return {};
}

std::unique_ptr<xtcash_contract_state> xtcash_contract_state::build_from(common::xaccount_address_t const & address, uint64_t const height) {
    auto * contract_state = new xtcash_contract_state{address};
    return std::unique_ptr<xtcash_contract_state>{contract_state};
}

std::unique_ptr<xtcash_contract_state> xtcash_contract_state::build_from(common::xaccount_address_t const & address, uint64_t const height, std::error_code & ec) {
    assert(!ec);

    try {
        return build_from(address, height);
    } catch (tcash::error::xtcash_error_t const & eh) {
        ec = eh.code();
    }

    return {};
}

common::xaccount_address_t xtcash_contract_state::state_account_address() const {
    assert(m_state_accessor != nullptr);
    return m_state_accessor->account_address();
}

uint64_t xtcash_contract_state::height() const {
    assert(m_state_accessor != nullptr);
    return m_state_accessor->height();
}

common::xtoken_t xtcash_contract_state::withdraw(state_accessor::properties::xproperty_identifier_t const & property_id,
                                                       common::xsymbol_t const & symbol,
                                                       evm_common::u256 amount,
                                                       std::error_code & ec) {
    assert(!ec);
    assert(m_state_accessor != nullptr);

    return m_state_accessor->withdraw(property_id, symbol, amount, ec);
}

common::xtoken_t xtcash_contract_state::withdraw(state_accessor::properties::xproperty_identifier_t const & property_id, common::xsymbol_t const & symbol, evm_common::u256 amount) {
    std::error_code ec;
    auto r = withdraw(property_id, symbol, amount, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::deposit(state_accessor::properties::xproperty_identifier_t const & property_id, common::xtoken_t tokens, std::error_code & ec) {
    assert(!ec);
    assert(m_state_accessor != nullptr);
    m_state_accessor->deposit(property_id, std::move(tokens), ec);
}

void xtcash_contract_state::deposit(state_accessor::properties::xproperty_identifier_t const & property_id, common::xtoken_t tokens) {
    std::error_code ec;
    deposit(property_id, std::move(tokens), ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

void xtcash_contract_state::create_property(state_accessor::properties::xproperty_identifier_t const & property_id, std::error_code & ec) {
    assert(m_state_accessor != nullptr);
    assert(!ec);
    m_state_accessor->create_property(property_id, ec);
}

void xtcash_contract_state::create_property(state_accessor::properties::xproperty_identifier_t const & property_id) {
    std::error_code ec;
    create_property(property_id, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

bool xtcash_contract_state::property_exist(state_accessor::properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    assert(m_state_accessor != nullptr);
    return m_state_accessor->property_exist(property_id, ec);
}

bool xtcash_contract_state::property_exist(state_accessor::properties::xproperty_identifier_t const & property_id) const {
    std::error_code ec;
    auto const r = property_exist(property_id, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::clear_property(state_accessor::properties::xproperty_identifier_t const & property_id, std::error_code & ec) {
    assert(!ec);
    assert(m_state_accessor != nullptr);
    m_state_accessor->clear_property(property_id, ec);
}

void xtcash_contract_state::clear_property(state_accessor::properties::xproperty_identifier_t const & property_id) {
    std::error_code ec;
    clear_property(property_id, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

size_t xtcash_contract_state::property_size(state_accessor::properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    assert(m_state_accessor != nullptr);
    return m_state_accessor->property_size(property_id, ec);
}

size_t xtcash_contract_state::property_size(state_accessor::properties::xproperty_identifier_t const & property_id) const {
    std::error_code ec;
    auto r = property_size(property_id, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::deploy_bin_code(state_accessor::properties::xproperty_identifier_t const & property_id, xbyte_buffer_t code, std::error_code & ec) {
    assert(!ec);
    assert(m_state_accessor != nullptr);

    m_state_accessor->deploy_bin_code(property_id, std::move(code), ec);
}

void xtcash_contract_state::deploy_bin_code(state_accessor::properties::xproperty_identifier_t const & property_id, xbyte_buffer_t code) {
    std::error_code ec;
    deploy_bin_code(property_id, std::move(code), ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

evm_common::u256 xtcash_contract_state::balance(state_accessor::properties::xproperty_identifier_t const & property_id,
                                              std::error_code & ec) const {
    assert(m_state_accessor != nullptr);
    assert(!ec);

    return m_state_accessor->balance(property_id, ec);
}

evm_common::u256 xtcash_contract_state::balance(state_accessor::properties::xproperty_identifier_t const & property_id) const {
    std::error_code ec;
    auto const r = balance(property_id, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

std::string xtcash_contract_state::binlog(std::error_code & ec) const {
    assert(m_state_accessor != nullptr);
    assert(!ec);
    return m_state_accessor->binlog(ec);
}

std::string xtcash_contract_state::binlog() const {
    std::error_code ec;
    auto r = binlog(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

size_t xtcash_contract_state::binlog_size(std::error_code & ec) const {
    assert(m_state_accessor != nullptr);
    assert(!ec);
    return m_state_accessor->binlog_size(ec);
}

size_t xtcash_contract_state::binlog_size() const {
    std::error_code ec;
    auto r = binlog_size(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

std::string xtcash_contract_state::fullstate_bin(std::error_code & ec) const {
    assert(m_state_accessor != nullptr);
    assert(!ec);
    return m_state_accessor->fullstate_bin(ec);
}

std::string xtcash_contract_state::fullstate_bin() const {
    std::error_code ec;
    auto r = fullstate_bin(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

common::xlogic_time_t xtcash_contract_state::time() const noexcept {
    return m_param.clock;
}

common::xlogic_time_t xtcash_contract_state::timestamp() const noexcept {
    return m_param.timestamp;
}

common::xlogic_time_t xtcash_contract_state::time_of_day() const noexcept {
    return m_param.timeofday;
}

uint64_t xtcash_contract_state::system_lock_tgas() const noexcept {
    return m_param.total_lock_tgas_token;
}

std::string const & xtcash_contract_state::random_seed() const noexcept {
    return m_param.random_seed;
}

uint256_t xtcash_contract_state::latest_sendtx_hash(std::error_code & ec) const {
    assert(!ec);
    // TODO(jimmy) XPROPERTY_TX_INFO should not be used
    auto r = get_property_cell_value<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_TX_INFO, state_accessor::properties::xproperty_category_t::system},
        data::XPROPERTY_TX_INFO_LATEST_SENDTX_HASH,
        ec);

    return r.empty() ? uint256_t{} : tcash::from_bytes<uint256_t>(r);
}

uint256_t xtcash_contract_state::latest_sendtx_hash() const {
    std::error_code ec;
    auto r = latest_sendtx_hash(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::latest_sendtx_hash(uint256_t hash, std::error_code & ec) {
    assert(!ec);
    // TODO(jimmy) XPROPERTY_TX_INFO should not be used
    set_property_cell_value<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_TX_INFO, state_accessor::properties::xproperty_category_t::system},
        data::XPROPERTY_TX_INFO_LATEST_SENDTX_HASH,
        tcash::to_bytes<uint256_t>(hash),
        ec);
}

void xtcash_contract_state::latest_sendtx_hash(uint256_t hash) {
    std::error_code ec;
    latest_sendtx_hash(hash, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

uint64_t xtcash_contract_state::latest_sendtx_nonce(std::error_code & ec) const {
    assert(!ec);
    // TODO(jimmy) XPROPERTY_TX_INFO should not be used
    auto r = get_property_cell_value<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_TX_INFO, state_accessor::properties::xproperty_category_t::system},
        data::XPROPERTY_TX_INFO_LATEST_SENDTX_NUM,
        ec);

    return r.empty() ? 0 : tcash::from_string<uint64_t>(tcash::from_bytes<std::string>(r));
}

uint64_t xtcash_contract_state::latest_sendtx_nonce() const {
    std::error_code ec;
    auto r = latest_sendtx_nonce(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::latest_sendtx_nonce(uint64_t nonce, std::error_code & ec) {
    assert(!ec);
    // TODO(jimmy) XPROPERTY_TX_INFO should not be used
    set_property_cell_value<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_TX_INFO, state_accessor::properties::xproperty_category_t::system},
        data::XPROPERTY_TX_INFO_LATEST_SENDTX_NUM,
        tcash::to_bytes<std::string>(tcash::to_string(nonce)),
        ec);
}

void xtcash_contract_state::latest_sendtx_nonce(uint64_t nonce) {
    std::error_code ec;
    latest_sendtx_nonce(nonce, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

uint256_t xtcash_contract_state::latest_followup_tx_hash() const {
    return m_latest_followup_tx_hash;
}

void xtcash_contract_state::latest_followup_tx_hash(uint256_t hash) {
    m_latest_followup_tx_hash = hash;
}

uint64_t  xtcash_contract_state::latest_followup_tx_nonce() const {
    return m_latest_followup_tx_nonce;
}

void xtcash_contract_state::latest_followup_tx_nonce(uint64_t nonce) {
    m_latest_followup_tx_nonce = nonce;
}

uint64_t xtcash_contract_state::unconfirm_sendtx_num(std::error_code & ec) const {
    assert(!ec);
    // TODO(jimmy) XPROPERTY_TX_INFO should not be used
    auto r = get_property_cell_value<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_TX_INFO, state_accessor::properties::xproperty_category_t::system},
        data::XPROPERTY_TX_INFO_UNCONFIRM_TX_NUM,
        ec);

    return r.empty() ? 0 : tcash::from_string<uint64_t>(tcash::from_bytes<std::string>(r));
}

uint64_t xtcash_contract_state::unconfirm_sendtx_num() const {
    std::error_code ec;
    auto const r = unconfirm_sendtx_num(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::unconfirm_sendtx_num(uint64_t num, std::error_code & ec) {
    assert(!ec);
    // TODO(jimmy) XPROPERTY_TX_INFO should not be used
    set_property_cell_value<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_TX_INFO, state_accessor::properties::xproperty_category_t::system},
        data::XPROPERTY_TX_INFO_UNCONFIRM_TX_NUM,
        tcash::to_bytes<std::string>(tcash::to_string(num)),
        ec);
}

void xtcash_contract_state::unconfirm_sendtx_num(uint64_t num) {
    std::error_code ec;
    unconfirm_sendtx_num(num, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

uint64_t xtcash_contract_state::used_tgas(std::error_code & ec) const {
    assert(!ec);
    auto r = get_property<state_accessor::properties::xproperty_type_t::string>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_USED_TGAS_KEY, state_accessor::properties::xproperty_category_t::system}, ec);
    
    return r.empty() ? 0 : tcash::from_string<uint64_t>(r);
}

uint64_t xtcash_contract_state::used_tgas() const {
    std::error_code ec;
    auto const r = used_tgas(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::used_tgas(uint64_t amount, std::error_code & ec) {
    assert(!ec);
    set_property<state_accessor::properties::xproperty_type_t::string>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_USED_TGAS_KEY, state_accessor::properties::xproperty_category_t::system},
        tcash::to_string(amount),
        ec);
}

void xtcash_contract_state::used_tgas(uint64_t amount) {
    std::error_code ec;
    used_tgas(amount, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

uint64_t xtcash_contract_state::lock_tgas(std::error_code & ec) const {
    assert(!ec);
    return get_property<state_accessor::properties::xproperty_type_t::uint64>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_LOCK_TGAS, state_accessor::properties::xproperty_category_t::system}, ec);
}

uint64_t xtcash_contract_state::lock_tgas() const {
    std::error_code ec;
    auto const r = lock_tgas(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::lock_tgas(uint64_t amount, std::error_code & ec) {
    assert(!ec);
    set_property<state_accessor::properties::xproperty_type_t::uint64>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_LOCK_TGAS, state_accessor::properties::xproperty_category_t::system}, amount, ec);
}

void xtcash_contract_state::lock_tgas(uint64_t amount) {
    std::error_code ec;
    lock_tgas(amount, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

uint64_t xtcash_contract_state::disk(std::error_code & ec) const {
    assert(!ec);
    return 0;
}

uint64_t xtcash_contract_state::disk() const {
    return 0;
}

void xtcash_contract_state::disk(uint64_t amount, std::error_code & ec) {
    assert(!ec);
}

void xtcash_contract_state::disk(uint64_t amount) {
}

uint64_t xtcash_contract_state::last_tx_hour(std::error_code & ec) const {
    assert(!ec);
    auto r = get_property<state_accessor::properties::xproperty_type_t::string>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_LAST_TX_HOUR_KEY, state_accessor::properties::xproperty_category_t::system}, ec);

    return r.empty() ? 0 : tcash::from_string<uint64_t>(r);
}

uint64_t xtcash_contract_state::last_tx_hour() const {
    std::error_code ec;
    auto const r = last_tx_hour(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::last_tx_hour(uint64_t hour, std::error_code & ec) {
    assert(!ec);
    set_property<state_accessor::properties::xproperty_type_t::string>(
        state_accessor::properties::xtypeless_property_identifier_t{data::XPROPERTY_LAST_TX_HOUR_KEY, state_accessor::properties::xproperty_category_t::system},
        tcash::to_string(hour),
        ec);
}

void xtcash_contract_state::last_tx_hour(uint64_t hour) {
    std::error_code ec;
    last_tx_hour(hour, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

void xtcash_contract_state::transfer_internal(state_accessor::properties::xproperty_identifier_t from, state_accessor::properties::xproperty_identifier_t to, uint64_t amount, std::error_code & ec) {
    auto withdraw_token = withdraw(from, common::SYMBOL_tcash_TOKEN, amount, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    deposit(to, std::move(withdraw_token), ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    xdbg("[xtcash_contract_state::transfer_internal] %s to %s success, amount: %" PRIu64, from.full_name().c_str(), to.full_name().c_str(), amount);
}

void xtcash_contract_state::transfer_internal(state_accessor::properties::xproperty_identifier_t from, state_accessor::properties::xproperty_identifier_t to, uint64_t amount) {
    std::error_code ec;
    transfer_internal(from, to, amount, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

std::map<std::string, data::system_contract::xreward_dispatch_task> xtcash_contract_state::delay_followup(std::error_code & ec) const {
    state_accessor::properties::xproperty_identifier_t property_id{
        data::system_contract::XPORPERTY_CONTRACT_TASK_KEY, state_accessor::properties::xproperty_type_t::map, state_accessor::properties::xproperty_category_t::user};
    if (!property_exist(property_id, ec)) {
        m_state_accessor->create_property(property_id, ec);
        if (ec) {
            return {};
        }
    }
    auto tasks_ser = get_property<state_accessor::properties::xproperty_type_t::map>(
        state_accessor::properties::xtypeless_property_identifier_t{data::system_contract::XPORPERTY_CONTRACT_TASK_KEY}, ec);
    if (ec) {
        return {};
    }
    std::map<std::string, data::system_contract::xreward_dispatch_task> res;
    for (auto const & pair : tasks_ser) {
        data::system_contract::xreward_dispatch_task task;
        auto str = tcash::from_bytes<std::string>(pair.second);
        base::xstream_t stream(base::xcontext_t::instance(), (uint8_t *)str.c_str(), (uint32_t)str.size());
        task.serialize_from(stream);
        res.insert(std::make_pair(pair.first, task));
    }
    return res;
}

std::map<std::string, data::system_contract::xreward_dispatch_task> xtcash_contract_state::delay_followup() const {
    std::error_code ec;
    auto r = delay_followup(ec);
    assert(!ec);
    tcash::error::throw_error(ec);
    return r;
}

void xtcash_contract_state::delay_followup(data::system_contract::xreward_dispatch_task const & task, std::error_code & ec) {
    auto const & tasks = delay_followup(ec);
    if (ec) {
        return;
    }
    uint32_t task_id = 0;
    if (tasks.size() > 0) {
        auto it = tasks.end();
        it--;
        task_id = tcash::from_string<uint32_t>(it->first);
        task_id++;
    }

    state_accessor::properties::xtypeless_property_identifier_t property{data::system_contract::XPORPERTY_CONTRACT_TASK_KEY};
    base::xstream_t stream(base::xcontext_t::instance());
    task.serialize_to(stream);
    std::stringstream ss;
    ss << std::setw(10) << std::setfill('0') << task_id;
    auto key = ss.str();
    auto value = std::string((char *)stream.data(), stream.size());
    m_state_accessor->set_property_cell_value<state_accessor::properties::xproperty_type_t::map>(property, key, tcash::to_bytes(value), ec);
}

void xtcash_contract_state::delay_followup(data::system_contract::xreward_dispatch_task const & task) {
    std::error_code ec;
    delay_followup(task, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

void xtcash_contract_state::delay_followup(std::vector<data::system_contract::xreward_dispatch_task> const & tasks, std::error_code & ec) {
    auto const & prev_tasks = delay_followup(ec);
    if (ec) {
        return;
    }
    uint32_t task_id = 0;
    if (prev_tasks.size() > 0) {
        auto it = prev_tasks.end();
        it--;
        task_id = tcash::from_string<uint32_t>(it->first);
        task_id++;
    }

    state_accessor::properties::xtypeless_property_identifier_t property{data::system_contract::XPORPERTY_CONTRACT_TASK_KEY};
    for (auto const & task : tasks) {
        base::xstream_t stream(base::xcontext_t::instance());
        task.serialize_to(stream);
        std::stringstream ss;
        ss << std::setw(10) << std::setfill('0') << task_id;
        auto key = ss.str();
        auto value = std::string((char *)stream.data(), stream.size());
        m_state_accessor->set_property_cell_value<state_accessor::properties::xproperty_type_t::map>(property, key, tcash::to_bytes(value), ec);
        if (ec) {
            return;
        }
        task_id++;
    }
}

void xtcash_contract_state::delay_followup(std::vector<data::system_contract::xreward_dispatch_task> const & tasks) {
    std::error_code ec;
    delay_followup(tasks, ec);
    assert(!ec);
    tcash::error::throw_error(ec);
}

NS_END2
