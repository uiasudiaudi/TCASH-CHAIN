// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv2/xelection_result_store.h"

#include "xbasic/xutility.h"

#include <exception>
#include <functional>
#include <iterator>
#include <utility>

NS_BEG4(tcash, data, election, v2)

std::map<common::xnetwork_id_t, xelection_network_result_t> const &
xtcash_election_result_store::results() const noexcept {
    return m_results;
}

void
xtcash_election_result_store::results(std::map<common::xnetwork_id_t, xelection_network_result_t> && r) noexcept {
    m_results = std::move(r);
}

xelection_network_result_t const &
xtcash_election_result_store::result_of(common::xnetwork_id_t const network_id) const {
    return m_results.at(network_id);
}

xelection_network_result_t &
xtcash_election_result_store::result_of(common::xnetwork_id_t const network_id) {
    return m_results[network_id];
}

std::size_t
xtcash_election_result_store::size_of(common::xnetwork_id_t const network_id) const noexcept {
    auto const it = m_results.find(network_id);
    if (it == std::end(m_results)) {
        return 0;
    }

    return tcash::get<xelection_network_result_t>(*it).size();
}

xtcash_election_result_store::iterator
xtcash_election_result_store::begin() noexcept {
    return m_results.begin();
}

xtcash_election_result_store::const_iterator
xtcash_election_result_store::begin() const noexcept {
    return m_results.begin();
}

xtcash_election_result_store::const_iterator
xtcash_election_result_store::cbegin() const noexcept {
    return m_results.cbegin();
}

xtcash_election_result_store::iterator
xtcash_election_result_store::end() noexcept {
    return m_results.end();
}

xtcash_election_result_store::const_iterator
xtcash_election_result_store::end() const noexcept {
    return m_results.end();
}

xtcash_election_result_store::const_iterator
xtcash_election_result_store::cend() const noexcept {
    return m_results.cend();
}

xtcash_election_result_store::iterator
xtcash_election_result_store::erase(const_iterator pos) {
    return m_results.erase(pos);
}

xtcash_election_result_store::size_type
xtcash_election_result_store::erase(key_type const & key) {
    return m_results.erase(key);
}

std::pair<xtcash_election_result_store::iterator, bool>
xtcash_election_result_store::insert(value_type const & value) {
    return m_results.insert(value);
}

std::pair<xtcash_election_result_store::iterator, bool>
xtcash_election_result_store::insert(value_type && value) {
    return m_results.insert(std::move(value));
}

std::size_t
xtcash_election_result_store::size() const noexcept {
    return m_results.size();
}

bool
xtcash_election_result_store::empty() const noexcept {
    return m_results.empty();
}

bool xtcash_election_result_store::empty_at(common::xnetwork_id_t const network_id) const noexcept {
    return m_results.find(network_id) == std::end(m_results);
}

v1::xelection_result_store_t xtcash_election_result_store::v1() const {
    v1::xelection_result_store_t r;

    std::transform(std::begin(m_results), std::end(m_results), std::inserter(r, std::end(r)), [](value_type const & input) -> v1::xelection_result_store_t::value_type {
        return {tcash::get<key_type const>(input), tcash::get<mapped_type>(input).v1()};
    });

    return r;
}

NS_END4
