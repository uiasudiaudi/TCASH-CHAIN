// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xdata/xblock.h"
#include "xdata/xdata_common.h"
#include "xvm/xcontract/xcontract_register.h"
#include "xvm/xsystem_contracts/xelection/xelect_nonconsensus_group_contract.h"

NS_BEG4(tcash, xvm, system_contracts, rec)

class xtcash_rec_elect_fullnode_contract final : public xvm::system_contracts::xelect_nonconsensus_group_contract_t {
    using xbase_t = xvm::system_contracts::xelect_nonconsensus_group_contract_t;

public:
    XDECLARE_DELETED_COPY_DEFAULTED_MOVE_SEMANTICS(xtcash_rec_elect_fullnode_contract);
    XDECLARE_DEFAULTED_OVERRIDE_DESTRUCTOR(xtcash_rec_elect_fullnode_contract);

    explicit xtcash_rec_elect_fullnode_contract(common::xnetwork_id_t const & network_id);

    xcontract_base * clone() override { return new xtcash_rec_elect_fullnode_contract(network_id()); }

    void setup() override;

    void on_timer(const uint64_t current_time);

    BEGIN_CONTRACT_WITH_PARAM(xtcash_rec_elect_fullnode_contract)
    CONTRACT_FUNCTION_PARAM(xtcash_rec_elect_fullnode_contract, on_timer);
    END_CONTRACT_WITH_PARAM

protected:
    common::xnode_type_t standby_type(common::xzone_id_t const & zid, common::xcluster_id_t const & cid, common::xgroup_id_t const & gid) const override;

private:
#ifdef STATIC_CONSENSUS
    void elect_config_nodes(common::xlogic_time_t const current_time);
#endif
};
using xrec_elect_fullnode_contract_t = xtcash_rec_elect_fullnode_contract;

NS_END4