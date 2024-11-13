// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xcrypto_key.h"
#include "xcommon/xlogic_time.h"
#include "xcommon/xnode_type.h"
#include "xcommon/xrole_type.h"
#include "xdata/xelection/xv0/xstandby_node_info.h"

#include <cstdint>
#include <map>

NS_BEG4(tcash, data, election, v1)

class xtcash_standby_node_info final {
public:
    std::map<common::xnode_type_t, uint64_t> stake_container;
    xpublic_key_t consensus_public_key{};  // public key for consensus business
    std::string program_version{};
    common::xminer_type_t miner_type{common::xminer_type_t::invalid};
    bool genesis{false};

    bool operator==(xtcash_standby_node_info const & other) const noexcept;

    bool operator!=(xtcash_standby_node_info const & other) const noexcept;

    void swap(xtcash_standby_node_info & other) noexcept;

    uint64_t stake(common::xnode_type_t const & node_type) const noexcept;

    void stake(common::xnode_type_t const & node_type, uint64_t stake) noexcept;

    v0::xstandby_node_info_t v0() const;
};
using xstandby_node_info_t = xtcash_standby_node_info;

NS_END4
