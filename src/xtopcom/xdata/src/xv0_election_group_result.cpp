// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv0/xelection_group_result.h"

#include "xbase/xlog.h"
#include "xbasic/xerror/xerror.h"
#include "xbasic/xutility.h"
#include "xdata/xerror/xerror.h"

NS_BEG4(tcash, data, election, v0)

bool xtcash_election_group_result::operator==(xtcash_election_group_result const & other) const noexcept {
    return m_nodes == other.m_nodes;
}
bool xtcash_election_group_result::operator!=(xtcash_election_group_result const & other) const noexcept {
    return !(*this == other);
}

common::xgroup_id_t const & xtcash_election_group_result::associated_group_id() const noexcept {
    return m_associated_group_id;
}

void xtcash_election_group_result::associated_group_id(common::xgroup_id_t gid) noexcept {
    if (m_associated_group_id != gid) {
        m_associated_group_id = std::move(gid);
    }
}

common::xelection_round_t const & xtcash_election_group_result::group_version() const noexcept {
    return m_group_version;
}

common::xelection_round_t & xtcash_election_group_result::group_version() noexcept {
    return m_group_version;
}

void xtcash_election_group_result::group_version(common::xelection_round_t ver) noexcept {
    if (m_group_version.empty() && ver.has_value()) {
        m_group_version = std::move(ver);
    } else if (m_group_version < ver) {
        m_group_version = std::move(ver);
    }
}

common::xelection_round_t const & xtcash_election_group_result::associated_group_version() const noexcept {
    return m_associated_group_version;
}

void xtcash_election_group_result::associated_group_version(common::xelection_round_t associated_gp_ver) noexcept {
    if (m_associated_group_version != associated_gp_ver) {
        m_associated_group_version = std::move(associated_gp_ver);
    }
}

common::xelection_round_t const & xtcash_election_group_result::cluster_version() const noexcept {
    return m_cluster_version;
}

void xtcash_election_group_result::cluster_version(common::xelection_round_t ver) noexcept {
    if (m_cluster_version != ver) {
        m_cluster_version = std::move(ver);
    }
}

common::xelection_round_t const & xtcash_election_group_result::election_committee_version() const noexcept {
    return m_election_committee_version;
}

common::xelection_round_t & xtcash_election_group_result::election_committee_version() noexcept {
    return m_election_committee_version;
}

void xtcash_election_group_result::election_committee_version(common::xelection_round_t committee_ver) noexcept {
    if (m_election_committee_version != committee_ver) {
        m_election_committee_version = std::move(committee_ver);
    }
}

common::xlogic_time_t xtcash_election_group_result::timestamp() const noexcept {
    return m_timestamp;
}

void xtcash_election_group_result::timestamp(common::xlogic_time_t const time) noexcept {
    m_timestamp = time;
}

common::xlogic_time_t xtcash_election_group_result::start_time() const {
    std::error_code ec;
    auto r = start_time(ec);
    tcash::error::throw_error(ec);
    return r;
}

common::xlogic_time_t xtcash_election_group_result::start_time(std::error_code & ec) const noexcept {
    if (m_start_time == common::xjudgement_day) {
        ec = tcash::data::error::xerrc_t::election_data_start_time_invalid;
        xwarn("%s ec %d ec msg %s", ec.category().name(), ec.value(), ec.message().c_str());
    }

    return m_start_time;
}

void xtcash_election_group_result::start_time(common::xlogic_time_t const time) {
    std::error_code ec;
    start_time(time, ec);
    tcash::error::throw_error(ec);
}

void xtcash_election_group_result::start_time(common::xlogic_time_t const time, std::error_code & ec) noexcept {
    if (time == common::xjudgement_day || (m_start_time != common::xjudgement_day && m_start_time >= time)) {
        ec = tcash::data::error::xerrc_t::election_data_start_time_invalid;
        xwarn("%s ec %d ec msg %s", ec.category().name(), ec.value(), ec.message().c_str());
        return;
    }

    m_start_time = time;
}

// std::uint64_t
// xtcash_election_group_result::associated_election_blk_height() const noexcept {
//    return m_associated_election_blk_height;
//}
//
// void
// xtcash_election_group_result::associated_election_blk_height(std::uint64_t const height) noexcept {
//    m_associated_election_blk_height = height;
//}

std::pair<xtcash_election_group_result::iterator, bool> xtcash_election_group_result::insert(xelection_info_bundle_t const & value) {
    // if the node already exists, do nothing.
    // if the node not in the result group yet, choose the first empty slot.

    auto find_result = find(value.node_id());
    if (tcash::get<bool>(find_result)) {
        return {m_nodes.find(tcash::get<common::xslot_id_t>(find_result)), false};
    }

    m_nodes[tcash::get<common::xslot_id_t>(find_result)] = value;
    return {m_nodes.find(tcash::get<common::xslot_id_t>(find_result)), true};
}

std::pair<xtcash_election_group_result::iterator, bool> xtcash_election_group_result::insert(xelection_info_bundle_t && value) {
    auto find_result = find(value.node_id());
    if (tcash::get<bool>(find_result)) {
        return {m_nodes.find(tcash::get<common::xslot_id_t>(find_result)), false};
    }

    m_nodes[tcash::get<common::xslot_id_t>(find_result)] = std::move(value);
    return {m_nodes.find(tcash::get<common::xslot_id_t>(find_result)), true};
}

xtcash_election_group_result::iterator xtcash_election_group_result::insert(const_iterator hint, value_type const & v) {
    return m_nodes.insert(hint, v);
}

xtcash_election_group_result::iterator xtcash_election_group_result::insert(const_iterator hint, value_type && v) {
    return m_nodes.insert(hint, std::move(v));
}

bool xtcash_election_group_result::empty() const noexcept {
    for (auto const & node_info : m_nodes) {
        auto const & election_info_bundle = tcash::get<xelection_info_bundle_t>(node_info);
        if (!election_info_bundle.node_id().empty()) {
            return false;
        }
    }

    return true;
}

std::map<common::xslot_id_t, xelection_info_bundle_t> const & xtcash_election_group_result::results() const noexcept {
    return m_nodes;
}

void xtcash_election_group_result::results(std::map<common::xslot_id_t, xelection_info_bundle_t> && r) noexcept {
    m_nodes = std::move(r);
}

xelection_info_t const & xtcash_election_group_result::result_of(common::xnode_id_t const & node_id) const {
    auto const find_result = find(node_id);
    if (!tcash::get<bool>(find_result)) {
        throw std::out_of_range{node_id.to_string()};
    }

    return m_nodes.at(tcash::get<common::xslot_id_t>(find_result)).election_info();
}

xelection_info_t & xtcash_election_group_result::result_of(common::xnode_id_t const & node_id) {
    auto find_result = find(node_id);

    auto & node_info = m_nodes[tcash::get<common::xslot_id_t>(find_result)];
    if (node_info.empty()) {
        node_info.node_id(node_id);
    }

    return node_info.election_info();
}

std::size_t xtcash_election_group_result::size() const noexcept {
    std::size_t size{0};
    for (auto const & node_info : m_nodes) {
        if (!tcash::get<xelection_info_bundle_t>(node_info).empty()) {
            ++size;
        }
    }

    return size;
}

xtcash_election_group_result::iterator xtcash_election_group_result::begin() noexcept {
    return m_nodes.begin();
}

xtcash_election_group_result::const_iterator xtcash_election_group_result::begin() const noexcept {
    return m_nodes.begin();
}

xtcash_election_group_result::const_iterator xtcash_election_group_result::cbegin() const noexcept {
    return m_nodes.cbegin();
}

xtcash_election_group_result::iterator xtcash_election_group_result::end() noexcept {
    return m_nodes.end();
}

xtcash_election_group_result::const_iterator xtcash_election_group_result::end() const noexcept {
    return m_nodes.end();
}

xtcash_election_group_result::const_iterator xtcash_election_group_result::cend() const noexcept {
    return m_nodes.cend();
}

void xtcash_election_group_result::reset(common::xnode_id_t const & node_id) {
    auto find_result = find(node_id);
    if (tcash::get<bool>(find_result)) {
        do_clear(tcash::get<common::xslot_id_t>(find_result));
    }
}

void xtcash_election_group_result::reset(iterator pos) {
    assert(pos != std::end(m_nodes));
    tcash::get<xelection_info_bundle_t>(*pos).clear();
}

void xtcash_election_group_result::clear(common::xslot_id_t const & slot_id) {
    assert(!broadcast(slot_id));
    auto const it = m_nodes.find(slot_id);
    if (it != std::end(m_nodes)) {
        do_clear(slot_id);
    }
}

std::pair<common::xslot_id_t, bool> xtcash_election_group_result::find(common::xnode_id_t const & nid) const noexcept {
    common::xslot_id_t reclaimed_slot_id;
    for (common::xslot_id_t slot_id{0}; slot_id < common::xslot_id_t{static_cast<common::xslot_id_t::value_type>(m_nodes.size())}; ++slot_id) {
        auto const & election_info_bundle = m_nodes.at(slot_id);

        if (election_info_bundle.empty()) {
            if (broadcast(reclaimed_slot_id)) {
                reclaimed_slot_id = slot_id;
            }
        } else if (election_info_bundle.node_id() == nid) {
            return {slot_id, true};
        }
    }

    if (broadcast(reclaimed_slot_id)) {
        reclaimed_slot_id = common::xslot_id_t{static_cast<common::xslot_id_t::value_type>(m_nodes.size())};
    }

    return {reclaimed_slot_id, false};
}

void xtcash_election_group_result::normalize() noexcept {
    auto iter_begin = begin();
    auto iter_end = std::prev(end());
    while (iter_begin != iter_end) {
        while (broadcast(tcash::get<common::xslot_id_t const>(*iter_end)) || (tcash::get<xelection_info_bundle_t>(*iter_end).empty() && iter_begin != iter_end)) {
            --iter_end;
        }
        while (!tcash::get<xelection_info_bundle_t>(*iter_begin).empty() && iter_begin != iter_end) {
            iter_begin++;
        }
        if (iter_begin == iter_end) {
            break;
        }

        tcash::get<xelection_info_bundle_t>(*iter_begin) = tcash::get<xelection_info_bundle_t>(std::move(*iter_end));
        reset(iter_end);
    }
    iter_end = std::prev(end());
    while (iter_end != begin() && tcash::get<xelection_info_bundle_t>(*iter_end).empty()) {
        m_nodes.erase(iter_end--);
    }
}

void xtcash_election_group_result::do_clear(common::xslot_id_t const & slot_id) {
    assert(!broadcast(slot_id));
    assert(m_nodes.find(slot_id) != std::end(m_nodes));
    m_nodes[slot_id].clear();
}

NS_END4
