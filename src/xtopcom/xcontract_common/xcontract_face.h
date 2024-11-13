// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"
#include "xbasic/xmemory.hpp"
#include "xcommon/xaddress.h"
#include "xcommon/xsymbol.h"
#include "xcontract_common/xcontract_execution_context.h"
#include "xcontract_common/xcontract_fwd.h"
#include "xcontract_common/xfollowup_transaction_datum.h"

#include <system_error>

NS_BEG2(tcash, contract_common)

class xtcash_contract_face {
public:
    xtcash_contract_face(xtcash_contract_face const &) = delete;
    xtcash_contract_face & operator=(xtcash_contract_face const &) = delete;
    xtcash_contract_face(xtcash_contract_face &&) = default;
    xtcash_contract_face & operator=(xtcash_contract_face &&) = default;
    virtual ~xtcash_contract_face() = default;

    virtual xcontract_execution_result_t execute(observer_ptr<xcontract_execution_context_t> exe_ctx) = 0;

    virtual common::xaccount_address_t sender() const = 0;
    virtual common::xaccount_address_t recver() const = 0;
    virtual common::xaccount_address_t address() const = 0;

    virtual void register_property(properties::xbasic_property_t * property) = 0;

    virtual common::xnetwork_id_t network_id() const = 0;
    virtual evm_common::u256 balance() const = 0;
    virtual common::xtoken_t withdraw(evm_common::u256 amount) = 0;
    virtual void deposit(common::xtoken_t token) = 0;

    virtual void reset_execution_context(observer_ptr<xcontract_execution_context_t> exe_ctx) = 0;

    virtual observer_ptr<xcontract_state_t> contract_state() const noexcept = 0;

protected:
    xtcash_contract_face() = default;

    virtual xbyte_buffer_t receipt_data(std::string const & key) const = 0;
    // virtual common::xtoken_t retrive_received_asset(std::string const & asset_id) const = 0;
    virtual void write_receipt_data(std::string const & key, xbyte_buffer_t value, std::error_code & ec) = 0;
    virtual void call(common::xaccount_address_t const & target_addr,
                      std::string const & method_name,
                      std::string const & method_params,
                      xfollowup_transaction_schedule_type_t type) = 0;
    virtual void call(common::xaccount_address_t const & target_addr,
                      std::string const & source_method_name,
                      std::string const & source_method_params,
                      std::string const & target_method_name,
                      std::string const & target_method_params,
                      xfollowup_transaction_schedule_type_t type) = 0;
};

NS_END2
