// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcontract_common/xstateless_contract_execution_context.h"

#include <system_error>

NS_BEG2(tcash, contract_common)

class xtcash_stateless_contract_face {
public:
    xtcash_stateless_contract_face(xtcash_stateless_contract_face const &) = delete;
    xtcash_stateless_contract_face & operator=(xtcash_stateless_contract_face const &) = delete;
    xtcash_stateless_contract_face(xtcash_stateless_contract_face &&) = default;
    xtcash_stateless_contract_face & operator=(xtcash_stateless_contract_face &&) = default;
    virtual ~xtcash_stateless_contract_face() = default;

    virtual xcontract_execution_result_t execute(observer_ptr<xstateless_contract_execution_context_t> exe_ctx) = 0;
    virtual void reset_execution_context(observer_ptr<xstateless_contract_execution_context_t> exe_ctx) = 0;

protected:
    xtcash_stateless_contract_face() = default;
};

NS_END2
