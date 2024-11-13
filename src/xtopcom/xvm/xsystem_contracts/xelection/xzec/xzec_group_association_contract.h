// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"
#include "xvm/xcontract/xcontract_base.h"
#include "xvm/xcontract/xcontract_exec.h"

NS_BEG4(tcash, xvm, system_contracts, zec)

class xtcash_group_association_contract final : public xcontract::xcontract_base {
    using xbase_t = xcontract::xcontract_base;

public:
    XDECLARE_DELETED_COPY_DEFAULTED_MOVE_SEMANTICS(xtcash_group_association_contract);
    XDECLARE_DEFAULTED_OVERRIDE_DESTRUCTOR(xtcash_group_association_contract);

    explicit
    xtcash_group_association_contract(common::xnetwork_id_t const & network_id);

    xcontract_base *
    clone() override;

    void
    setup() override;

    BEGIN_CONTRACT_WITH_PARAM(xtcash_group_association_contract)
    END_CONTRACT_WITH_PARAM
};
using xgroup_association_contract_t = xtcash_group_association_contract;

NS_END4
