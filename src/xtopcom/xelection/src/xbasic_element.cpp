// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xelection/xcache/xbasic_element.h"

NS_BEG3(tcash, election, cache)

xtcash_basic_element::xtcash_basic_element(common::xnetwork_id_t const & network_id)
    : m_address{ common::xcluster_address_t{ network_id } } {
}

xtcash_basic_element::xtcash_basic_element(common::xnetwork_id_t const & network_id,
                                       common::xzone_id_t const & zone_id)
    : m_address{ common::xcluster_address_t{ network_id, zone_id } } {
}

xtcash_basic_element::xtcash_basic_element(common::xnetwork_id_t const & network_id,
                                       common::xzone_id_t const & zone_id,
                                       common::xcluster_id_t const & cluster_id)
    : m_address{
        common::xcluster_address_t{
            network_id,
            zone_id,
            cluster_id
        }
    } {
}

xtcash_basic_element::xtcash_basic_element(common::xelection_round_t const & election_round,
                                       common::xnetwork_id_t const & network_id,
                                       common::xzone_id_t const & zone_id,
                                       common::xcluster_id_t const & cluster_id,
                                       common::xgroup_id_t const & group_id,
                                       uint16_t const group_size,
                                       uint64_t const associated_blk_height)
    : m_address{
        common::xcluster_address_t{
            network_id,
            zone_id,
            cluster_id,
            group_id
        },
        election_round,
        group_size,
        associated_blk_height
    } {
}

xtcash_basic_element::xtcash_basic_element(common::xelection_round_t const & election_round,
                                       common::xnetwork_id_t const & network_id,
                                       common::xzone_id_t const & zone_id,
                                       common::xcluster_id_t const & cluster_id,
                                       common::xgroup_id_t const & group_id,
                                       common::xnode_id_t const & node_id,
                                       common::xslot_id_t const & slot_id,
                                       uint16_t const group_size,
                                       uint64_t const associated_blk_height)
    : m_address{
        common::xcluster_address_t{
            network_id,
            zone_id,
            cluster_id,
            group_id
        },
        common::xaccount_election_address_t{
            node_id,
            slot_id
        },
        election_round,
        group_size,
        associated_blk_height
    } {
}

common::xnode_address_t const &
xtcash_basic_element::address() const noexcept {
    return m_address;
}

common::xip2_t xtcash_basic_element::xip2() const {
    return address().xip2();
}

common::xcluster_id_t
xtcash_basic_element::cluster_id() const noexcept {
    return m_address.cluster_id();
}

common::xgroup_id_t
xtcash_basic_element::group_id() const noexcept {
    return m_address.group_id();
}

common::xnetwork_id_t
xtcash_basic_element::network_id() const noexcept {
    return m_address.network_id();
}

common::xnode_id_t const &
xtcash_basic_element::node_id() const noexcept {
    return m_address.node_id();
}

common::xaccount_address_t const & xtcash_basic_element::account_address() const noexcept {
    return m_address.account_address();
}

bool
xtcash_basic_element::operator==(xtcash_basic_element const & other) const noexcept {
    return m_address == other.m_address;
}

common::xslot_id_t
xtcash_basic_element::slot_id() const noexcept {
    return m_address.slot_id();
}

uint16_t xtcash_basic_element::group_size() const noexcept {
    return m_address.group_size();
}

uint64_t
xtcash_basic_element::associated_blk_height() const noexcept {
    return m_address.associated_blk_height();
}

common::xnode_type_t
xtcash_basic_element::type() const noexcept {
    return m_address.type();
}

void
xtcash_basic_element::swap(xtcash_basic_element & other) noexcept {
    m_address.swap(other.m_address);
}

common::xelection_round_t const & xtcash_basic_element::election_round() const noexcept {
    return m_address.election_round();
}

common::xlogical_version_t const & xtcash_basic_element::logic_epoch() const noexcept {
    return m_address.logic_epoch();
}

common::xzone_id_t
xtcash_basic_element::zone_id() const noexcept {
    return m_address.zone_id();
}

NS_END3
