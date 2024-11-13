// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xsystem_contracts/xbasic_system_contract.h"

NS_BEG3(tcash, contract_runtime, system)

class xtcash_contract_object_creator {
public:
    xtcash_contract_object_creator() = default;
    xtcash_contract_object_creator(xtcash_contract_object_creator const &) = delete;
    xtcash_contract_object_creator & operator=(xtcash_contract_object_creator const &) = delete;
    xtcash_contract_object_creator(xtcash_contract_object_creator &&) = default;
    xtcash_contract_object_creator & operator=(xtcash_contract_object_creator &&) = delete;
    virtual ~xtcash_contract_object_creator() = default;

    virtual std::unique_ptr<system_contracts::xbasic_system_contract_t> create() = 0;
};
using xcontract_object_creator_t = xtcash_contract_object_creator;

template <typename SystemContractT>
class xtcash_system_contract_object_creator : public xcontract_object_creator_t {
public:
    std::unique_ptr<system_contracts::xbasic_system_contract_t> create() override {
        auto * system_contract_obj = new SystemContractT{};
        return std::unique_ptr<SystemContractT>{system_contract_obj};
    }
};

template <typename SystemContractT>
using xsystem_contract_object_creator_t = xtcash_system_contract_object_creator<SystemContractT>;

NS_END3
