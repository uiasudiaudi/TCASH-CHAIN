// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv1/xstandby_node_info.h"

NS_BEG4(tcash, data, election, v1)

bool xtcash_standby_node_info::operator==(xtcash_standby_node_info const & other) const noexcept {
    return consensus_public_key == other.consensus_public_key && stake_container == other.stake_container && program_version == other.program_version &&
           miner_type == other.miner_type && genesis == other.genesis;
}

bool xtcash_standby_node_info::operator!=(xtcash_standby_node_info const & other) const noexcept {
    return !(*this == other);
}

void xtcash_standby_node_info::swap(xtcash_standby_node_info & other) noexcept {
    consensus_public_key.swap(other.consensus_public_key);
    std::swap(stake_container, other.stake_container);
    std::swap(program_version, other.program_version);
    std::swap(genesis, other.genesis);
    std::swap(miner_type, other.miner_type);
}

uint64_t xtcash_standby_node_info::stake(common::xnode_type_t const & node_type) const noexcept {
    if (stake_container.find(node_type) == stake_container.end()) {
        xassert(false);
        return 0;
    } else {
        return stake_container.at(node_type);
    }
}

void xtcash_standby_node_info::stake(common::xnode_type_t const & node_type, uint64_t new_stake) noexcept {
    stake_container[node_type] = new_stake;
}

v0::xstandby_node_info_t xtcash_standby_node_info::v0() const {
    v0::xstandby_node_info_t r;
    r.stake_container = stake_container;
    r.consensus_public_key = consensus_public_key;
    r.program_version = program_version;
    r.genesis = genesis;

    return r;
}

NS_END4
