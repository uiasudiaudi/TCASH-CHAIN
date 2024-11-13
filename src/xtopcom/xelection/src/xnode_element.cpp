// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xelection/xcache/xgroup_element.h"
#include "xelection/xcache/xnode_element.h"

#if defined(DEBUG)
#   include <cinttypes>
#endif

NS_BEG3(tcash, election, cache)

xtcash_node_element::xtcash_node_element(common::xnode_id_t const & node_id,
                                     common::xslot_id_t const & slot_id,
                                     data::election::xelection_info_t const & election_info,
                                     std::shared_ptr<xgroup_element_t> const & group_element)
    : xbase_t{
        group_element->election_round(),
        group_element->address().network_id(),
        group_element->address().zone_id(),
        group_element->address().cluster_id(),
        group_element->address().group_id(),
        node_id,
        slot_id,
        group_element->group_size(),
        group_element->associated_blk_height()
    }
    , m_group_element{ group_element }
    , m_election_info{ election_info } {
}

std::shared_ptr<xgroup_element_t>
xtcash_node_element::group_element() const noexcept {
    return m_group_element.lock();
}

data::election::xelection_info_t const &
xtcash_node_element::election_info() const noexcept {
    return m_election_info;
}

xtcash_node_element::hash_result_type
xtcash_node_element::hash() const {
    return node_id().hash();
}

common::xelection_round_t const &
xtcash_node_element::joined_election_round() const noexcept {
    return m_election_info.joined_epoch();
}

std::uint64_t
xtcash_node_element::staking() const noexcept {
    return m_election_info.stake();
}

std::uint64_t xtcash_node_element::raw_credit_score() const noexcept {
    xdbg("xnode_element_t::raw_credit_score %" PRIu64, m_election_info.raw_credit_score());
    return m_election_info.raw_credit_score();
}

bool
xtcash_node_element::operator==(xtcash_node_element const & other) const noexcept {
    if (!xbase_t::operator==(other)) {
        return false;
    }

    return m_election_info == other.m_election_info;
}

bool
xtcash_node_element::operator!=(xtcash_node_element const & other) const noexcept {
    return !(*this == other);
}

void
xtcash_node_element::swap(xtcash_node_element & other) noexcept {
    m_group_element.swap(other.m_group_element);
    m_election_info.swap(other.m_election_info);
    xbase_t::swap(other);
}

NS_END3
