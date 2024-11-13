// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xelection/xv2/xelection_network_result.h"

#include "xbasic/xutility.h"

#include <exception>
#include <functional>
#include <iterator>
#include <utility>

NS_BEG4(tcash, data, election, v2)

std::map<common::xnode_type_t, xelection_result_t> const &
xtcash_election_network_result::results() const noexcept {
    return m_results;
}

void
xtcash_election_network_result::results(std::map<common::xnode_type_t, xtcash_election_result> && r) noexcept {
    m_results = std::move(r);
}

xelection_result_t const &
xtcash_election_network_result::result_of(common::xnode_type_t const type) const {
    return m_results.at(type);
}

xelection_result_t &
xtcash_election_network_result::result_of(common::xnode_type_t const type) {
    return m_results[type];
}

std::size_t
xtcash_election_network_result::size_of(common::xnode_type_t const type) const noexcept {
    auto const it = m_results.find(type);
    if (it == std::end(m_results)) {
        return 0;
    }

    return tcash::get<xelection_result_t>(*it).size();
}

xtcash_election_network_result::iterator
xtcash_election_network_result::begin() noexcept {
    return m_results.begin();
}

xtcash_election_network_result::const_iterator
xtcash_election_network_result::begin() const noexcept {
    return m_results.begin();
}

xtcash_election_network_result::const_iterator
xtcash_election_network_result::cbegin() const noexcept {
    return m_results.cbegin();
}

xtcash_election_network_result::iterator
xtcash_election_network_result::end() noexcept {
    return m_results.end();
}

xtcash_election_network_result::const_iterator
xtcash_election_network_result::end() const noexcept {
    return m_results.end();
}

xtcash_election_network_result::const_iterator
xtcash_election_network_result::cend() const noexcept {
    return m_results.cend();
}

xtcash_election_network_result::iterator
xtcash_election_network_result::erase(const_iterator pos) {
    return m_results.erase(pos);
}

xtcash_election_network_result::size_type
xtcash_election_network_result::erase(key_type const & key) {
    return m_results.erase(key);
}

std::pair<xtcash_election_network_result::iterator, bool>
xtcash_election_network_result::insert(value_type const & value) {
    return m_results.insert(value);
}

std::pair<xtcash_election_network_result::iterator, bool>
xtcash_election_network_result::insert(value_type && value) {
    return m_results.insert(std::move(value));
}

std::size_t
xtcash_election_network_result::size() const noexcept {
    return m_results.size();
}

v1::xelection_network_result_t xtcash_election_network_result::v1() const {
    v1::xelection_network_result_t r;

    std::transform(std::begin(m_results), std::end(m_results), std::inserter(r, std::end(r)), [](value_type const & input) -> v1::xelection_network_result_t::value_type {
        return {tcash::get<key_type const>(input), tcash::get<mapped_type>(input).v1()};
    });

    return r;
}

bool xtcash_election_network_result::empty() const noexcept {
    return m_results.empty();
}

bool xtcash_election_network_result::empty_at(common::xnode_type_t const type) const noexcept {
    return m_results.find(type) == std::end(m_results);
}


NS_END4
