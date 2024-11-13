// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcontract_common/xcontract_face.h"
#include "xcontract_common/xcontract_fwd.h"
#include "xcontract_common/xstateless_contract_face.h"

NS_BEG2(tcash, contract_common)

class xtcash_basic_stateless_contract : public xstateless_contract_face_t {
protected:
    observer_ptr<xstateless_contract_execution_context_t> m_associated_execution_context{nullptr};

public:
    xtcash_basic_stateless_contract() = default;
    xtcash_basic_stateless_contract(xtcash_basic_stateless_contract const &) = delete;
    xtcash_basic_stateless_contract & operator=(xtcash_basic_stateless_contract const &) = delete;
    xtcash_basic_stateless_contract(xtcash_basic_stateless_contract &&) = default;
    xtcash_basic_stateless_contract & operator=(xtcash_basic_stateless_contract &&) = default;
    ~xtcash_basic_stateless_contract() override = default;

    observer_ptr<data::xunit_bstate_t> unitstate_owned() const noexcept {
        return m_associated_execution_context->unitstate_owned();
    }

    observer_ptr<data::xunit_bstate_t> unitstate_other() const noexcept {
        return m_associated_execution_context->unitstate_other();
    }

    void reset_execution_context(observer_ptr<xstateless_contract_execution_context_t> exe_ctx) override final {
        m_associated_execution_context = exe_ctx;
    }
};

NS_END2
