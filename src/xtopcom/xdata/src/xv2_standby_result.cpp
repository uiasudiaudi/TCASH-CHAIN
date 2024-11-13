// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv2/xstandby_result.h"

NS_BEG4(tcash, data, election, v2)

std::pair<xtcash_standby_result::iterator, bool>
xtcash_standby_result::update(value_type const & value) {
    if (m_nodes.find(value.first) == m_nodes.end() || m_nodes[value.first] == value.second) {
        return m_nodes.insert(value);
    } else {
        // m_nodes[value.first] != value.second
        m_nodes.erase(value.first);
        return m_nodes.insert(value);
    }
}

std::pair<xtcash_standby_result::iterator, bool>
xtcash_standby_result::update(value_type && value) {
    if (m_nodes.find(value.first) == m_nodes.end() || m_nodes[value.first] == value.second) {
        return m_nodes.insert(value);
    } else {
        // m_nodes[value.first] != value.second
        m_nodes.erase(value.first);
        return m_nodes.insert(std::move(value));
    }
}

std::pair<xtcash_standby_result::iterator, bool>
xtcash_standby_result::insert(value_type const & value) {
    return m_nodes.insert(value);
}

std::pair<xtcash_standby_result::iterator, bool>
xtcash_standby_result::insert(value_type && value) {
    return m_nodes.insert(std::move(value));
}

bool
xtcash_standby_result::empty() const noexcept {
    return m_nodes.empty();
}

std::map<common::xnode_id_t, xstandby_node_info_t> const &
xtcash_standby_result::results() const noexcept {
    return m_nodes;
}

std::map<common::xnode_id_t, xstandby_node_info_t> &
xtcash_standby_result::results() noexcept {
    return m_nodes;
}

void
xtcash_standby_result::results(std::map<common::xnode_id_t, xstandby_node_info_t> && r) noexcept {
    m_nodes = std::move(r);
}

xstandby_node_info_t const &
xtcash_standby_result::result_of(common::xnode_id_t const & nid) const {
    return m_nodes.at(nid);
}

xstandby_node_info_t &
xtcash_standby_result::result_of(common::xnode_id_t const & nid) {
    return m_nodes[nid];
}

std::size_t
xtcash_standby_result::size() const noexcept {
    return m_nodes.size();
}

xtcash_standby_result::const_iterator
xtcash_standby_result::begin() const noexcept {
    return m_nodes.begin();
}

xtcash_standby_result::iterator
xtcash_standby_result::begin() noexcept {
    return m_nodes.begin();
}

xtcash_standby_result::const_iterator
xtcash_standby_result::cbegin() const noexcept {
    return m_nodes.cbegin();
}

xtcash_standby_result::const_iterator
xtcash_standby_result::end() const noexcept {
    return m_nodes.end();
}

xtcash_standby_result::iterator
xtcash_standby_result::end() noexcept {
    return m_nodes.end();
}

xtcash_standby_result::const_iterator
xtcash_standby_result::cend() const noexcept {
    return m_nodes.cend();
}

xtcash_standby_result::const_iterator
xtcash_standby_result::find(common::xnode_id_t const & id) const noexcept {
    return m_nodes.find(id);
}

xtcash_standby_result::iterator
xtcash_standby_result::erase(const_iterator pos) {
    return m_nodes.erase(pos);
}

xtcash_standby_result::size_type
xtcash_standby_result::erase(key_type const & key) {
    return m_nodes.erase(key);
}

NS_END4
