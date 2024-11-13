// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv2/xelection_result.h"

#include "xbasic/xutility.h"

#include <functional>
#include <iterator>

NS_BEG4(tcash, data, election, v2)

std::pair<xtcash_election_result::iterator, bool>
xtcash_election_result::insert(value_type const & value) {
    return m_cluster_results.insert(value);
}

std::pair<xtcash_election_result::iterator, bool>
xtcash_election_result::insert(value_type && value) {
    return m_cluster_results.insert(std::move(value));
}

bool
xtcash_election_result::empty() const noexcept {
    return m_cluster_results.empty();
}

std::map<common::xcluster_id_t, xelection_cluster_result_t> const &
xtcash_election_result::results() const noexcept {
    return m_cluster_results;
}

void
xtcash_election_result::results(std::map<common::xcluster_id_t, xelection_cluster_result_t> && r) noexcept {
    m_cluster_results = std::move(r);
}

xelection_cluster_result_t const &
xtcash_election_result::result_of(common::xcluster_id_t const & cid) const {
    return m_cluster_results.at(cid);
}

xelection_cluster_result_t &
xtcash_election_result::result_of(common::xcluster_id_t const & cid) {
    return m_cluster_results[cid];
}

std::size_t
xtcash_election_result::size() const noexcept {
    return m_cluster_results.size();
}

xtcash_election_result::iterator
xtcash_election_result::begin() noexcept {
    return m_cluster_results.begin();
}

xtcash_election_result::const_iterator
xtcash_election_result::begin() const noexcept {
    return m_cluster_results.begin();
}

xtcash_election_result::const_iterator
xtcash_election_result::cbegin() const noexcept {
    return m_cluster_results.cbegin();
}

xtcash_election_result::iterator
xtcash_election_result::end() noexcept {
    return m_cluster_results.end();
}

xtcash_election_result::const_iterator
xtcash_election_result::end() const noexcept {
    return m_cluster_results.end();
}

xtcash_election_result::const_iterator
xtcash_election_result::cend() const noexcept {
    return m_cluster_results.cend();
}

xtcash_election_result::iterator
xtcash_election_result::erase(const_iterator pos) {
    return m_cluster_results.erase(pos);
}

xtcash_election_result::size_type
xtcash_election_result::erase(key_type const & key) {
    return m_cluster_results.erase(key);
}

v1::xelection_result_t xtcash_election_result::v1() const {
    v1::xelection_result_t r;

    std::transform(
        std::begin(m_cluster_results), std::end(m_cluster_results), std::inserter(r, std::end(r)), [](value_type const & input) -> v1::xelection_result_t::value_type {
            return {tcash::get<key_type const>(input), tcash::get<mapped_type>(input).v1()};
        });

    return r;
}

bool xelection_result_t::empty_at(common::xcluster_id_t const cluster_id) const noexcept {
    return m_cluster_results.find(cluster_id) == std::end(m_cluster_results);
}

NS_END4
