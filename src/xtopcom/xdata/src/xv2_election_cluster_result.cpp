// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv2/xelection_cluster_result.h"

#include "xbasic/xutility.h"

#include <functional>
#include <iterator>

NS_BEG4(tcash, data, election, v2)

xtcash_election_cluster_result::iterator
xtcash_election_cluster_result::begin() noexcept {
    return m_group_results.begin();
}

xtcash_election_cluster_result::const_iterator
xtcash_election_cluster_result::begin() const noexcept {
    return m_group_results.begin();
}

xtcash_election_cluster_result::const_iterator
xtcash_election_cluster_result::cbegin() const noexcept {
    return m_group_results.cbegin();
}

xtcash_election_cluster_result::const_iterator
xtcash_election_cluster_result::cend() const noexcept {
    return m_group_results.cend();
}

bool
xtcash_election_cluster_result::empty() const noexcept {
    return m_group_results.empty();
}

bool xtcash_election_cluster_result::empty_at(common::xgroup_id_t const & group_id) const noexcept {
    return m_group_results.find(group_id) == std::end(m_group_results);
}

xtcash_election_cluster_result::iterator
xtcash_election_cluster_result::end() noexcept {
    return m_group_results.end();
}

xtcash_election_cluster_result::const_iterator
xtcash_election_cluster_result::end() const noexcept {
    return m_group_results.end();
}

std::map<common::xgroup_id_t, xelection_group_result_t> const &
xtcash_election_cluster_result::results() const noexcept {
    return m_group_results;
}

void
xtcash_election_cluster_result::results(std::map<common::xgroup_id_t, xelection_group_result_t> && r) noexcept {
    m_group_results = std::move(r);
}

std::size_t
xtcash_election_cluster_result::size() const noexcept {
    return m_group_results.size();
}

xelection_group_result_t &
xtcash_election_cluster_result::result_of(common::xgroup_id_t const & gid) {
    return m_group_results[gid];
}

xelection_group_result_t const &
xtcash_election_cluster_result::result_of(common::xgroup_id_t const & gid) const {
    return m_group_results.at(gid);
}

std::pair<xtcash_election_cluster_result::iterator, bool>
xtcash_election_cluster_result::insert(value_type && value) {
    return m_group_results.insert(std::move(value));
}

std::pair<xtcash_election_cluster_result::iterator, bool>
xtcash_election_cluster_result::insert(value_type const & value) {
    return m_group_results.insert(value);
}

xtcash_election_cluster_result::size_type
xtcash_election_cluster_result::erase(key_type const & key) {
    return m_group_results.erase(key);
}

xtcash_election_cluster_result::iterator
xtcash_election_cluster_result::erase(const_iterator pos) {
    return m_group_results.erase(pos);
}

v1::xelection_cluster_result_t xtcash_election_cluster_result::v1() const {
    v1::xelection_cluster_result_t r;

    std::transform(
        std::begin(m_group_results), std::end(m_group_results), std::inserter(r, std::end(r)), [](value_type const & input) -> v1::xelection_cluster_result_t::value_type {
            return {tcash::get<key_type const>(input), tcash::get<mapped_type>(input).v1()};
        });

    return r;
}

NS_END4
