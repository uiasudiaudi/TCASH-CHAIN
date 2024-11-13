﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xip.h"
#include "xdata/xelection/xv0/xelection_cluster_result.h"

#include <map>
#include <string>

NS_BEG4(tcash, data, election, v0)

class xtcash_election_result final {
private:
    using container_t = std::map<common::xcluster_id_t, xelection_cluster_result_t>;
    container_t m_cluster_results{};

public:
    using key_type        = container_t::key_type;
    using mapped_type     = container_t::mapped_type;
    using value_type      = container_t::value_type;
    using size_type       = container_t::size_type;
    using difference_type = container_t::difference_type;
    using key_compare     = container_t::key_compare;
    using reference       = container_t::reference;
    using const_reference = container_t::const_reference;
    using pointer         = container_t::pointer;
    using const_pointer   = container_t::const_pointer;
    using iterator        = container_t::iterator;
    using const_iterator  = container_t::const_iterator;

    std::pair<iterator, bool>
    insert(value_type const & value);

    std::pair<iterator, bool>
    insert(value_type && value);

    iterator insert(const_iterator hint, value_type const & v);
    iterator insert(const_iterator hint, value_type && v);

    bool
    empty() const noexcept;

    std::map<common::xcluster_id_t, xelection_cluster_result_t> const &
    results() const noexcept;

    void
    results(std::map<common::xcluster_id_t, xelection_cluster_result_t> && r) noexcept;

    xelection_cluster_result_t const &
    result_of(common::xcluster_id_t const & cid) const;

    xelection_cluster_result_t &
    result_of(common::xcluster_id_t const & cid);

    std::size_t
    size() const noexcept;

    iterator
    begin() noexcept;

    const_iterator
    begin() const noexcept;

    const_iterator
    cbegin() const noexcept;

    iterator
    end() noexcept;

    const_iterator
    end() const noexcept;

    const_iterator
    cend() const noexcept;

    iterator
    erase(const_iterator pos);

    size_type
    erase(key_type const & key);
};
using xelection_result_t = xtcash_election_result;

NS_END4
