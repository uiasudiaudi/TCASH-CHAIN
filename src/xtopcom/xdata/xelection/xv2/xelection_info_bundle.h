// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xnode_id.h"
#include "xdata/xelection/xv1/xelection_info_bundle.h"
#include "xdata/xelection/xv2/xelection_info.h"

NS_BEG4(tcash, data, election, v2)

class xtcash_election_info_bundle final {
private:
    common::xnode_id_t m_node_id{};
    xelection_info_t m_election_info{};

public:
    bool 
    operator==(xtcash_election_info_bundle const & other) const noexcept;

    bool 
    operator!=(xtcash_election_info_bundle const & other) const noexcept;

    common::xaccount_address_t const & account_address() const noexcept;
    common::xaccount_address_t & account_address() noexcept;
    void account_address(common::xaccount_address_t && addr) noexcept;
    void account_address(common::xaccount_address_t const & addr);

    xelection_info_t const &
    election_info() const noexcept;

    xelection_info_t &
    election_info() noexcept;

    void
    election_info(xelection_info_t && info) noexcept;

    void
    election_info(xelection_info_t const & info);

    void
    clear() noexcept;

    bool
    empty() const noexcept;

    v1::xelection_info_bundle_t v1() const;
};
using xelection_info_bundle_t = xtcash_election_info_bundle;

NS_END4