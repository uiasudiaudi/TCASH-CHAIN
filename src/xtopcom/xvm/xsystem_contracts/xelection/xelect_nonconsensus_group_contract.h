// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "xvm/xsystem_contracts/xelection/xelect_group_contract.h"

NS_BEG3(tcash, xvm, system_contracts)

class xtcash_elect_nonconsensus_group_contract : public xelect_group_contract_t {
    using xbase_t = xelect_group_contract_t;

public:
    XDECLARE_DELETED_COPY_DEFAULTED_MOVE_SEMANTICS(xtcash_elect_nonconsensus_group_contract);
    XDECLARE_DEFAULTED_OVERRIDE_DESTRUCTOR(xtcash_elect_nonconsensus_group_contract);

protected:
    explicit xtcash_elect_nonconsensus_group_contract(common::xnetwork_id_t const & network_id);

public:
    bool elect_group(common::xzone_id_t const & zid,
                     common::xcluster_id_t const & cid,
                     common::xgroup_id_t const & gid,
                     common::xlogic_time_t election_timestamp,
                     common::xlogic_time_t start_time,
                     xrange_t<config::xgroup_size_t> const & group_size_range,
                     bool force_update,
                     data::election::xstandby_network_result_t & standby_network_result,
                     data::election::xelection_network_result_t & election_network_result) override;

protected:
    virtual common::xnode_type_t standby_type(common::xzone_id_t const & zid, common::xcluster_id_t const & cid, common::xgroup_id_t const & gid) const = 0;
};
using xelect_nonconsensus_group_contract_t = xtcash_elect_nonconsensus_group_contract;

NS_END3
