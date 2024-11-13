// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xaddress.h"
#include "xcommon/xnode_type.h"
#include "xcommon/xversion.h"

NS_BEG3(tcash, election, cache)

class xtcash_basic_element {
    common::xnode_address_t m_address;

protected:
    /**
     * @brief Construct a network data element
     */
    explicit xtcash_basic_element(common::xnetwork_id_t const & network_id);

    /**
     * @brief Construct a zone data element
     */
    xtcash_basic_element(common::xnetwork_id_t const & network_id, common::xzone_id_t const & zone_id);

    /**
     * @brief Construct a cluster data element
     */
    xtcash_basic_element(common::xnetwork_id_t const & network_id, common::xzone_id_t const & zone_id, common::xcluster_id_t const & cluster_id);

    xtcash_basic_element(common::xelection_round_t const & election_round,
                       common::xnetwork_id_t const & network_id,
                       common::xzone_id_t const & zone_id,
                       common::xcluster_id_t const & cluster_id,
                       common::xgroup_id_t const & group_id,
                       uint16_t const group_size,
                       uint64_t const associated_blk_height);

    xtcash_basic_element(common::xelection_round_t const & election_round,
                       common::xnetwork_id_t const & network_id,
                       common::xzone_id_t const & zone_id,
                       common::xcluster_id_t const & cluster_id,
                       common::xgroup_id_t const & group_id,
                       common::xnode_id_t const & node_id,
                       common::xslot_id_t const & slot_id,
                       uint16_t const group_size,
                       uint64_t const associated_blk_height);

public:
    xtcash_basic_element(xtcash_basic_element const &) = delete;
    xtcash_basic_element & operator=(xtcash_basic_element const &) = delete;
    xtcash_basic_element(xtcash_basic_element &&) = default;
    xtcash_basic_element & operator=(xtcash_basic_element &&) = default;
    virtual ~xtcash_basic_element() = default;

    common::xnode_type_t type() const noexcept;

    common::xelection_round_t const & election_round() const noexcept;

    common::xlogical_version_t const & logic_epoch() const noexcept;

    common::xnetwork_id_t network_id() const noexcept;

    common::xzone_id_t zone_id() const noexcept;

    common::xcluster_id_t cluster_id() const noexcept;

    common::xgroup_id_t group_id() const noexcept;

    common::xslot_id_t slot_id() const noexcept;

    uint16_t group_size() const noexcept;

    uint64_t associated_blk_height() const noexcept;

    common::xnode_id_t const & node_id() const noexcept;

    common::xaccount_address_t const & account_address() const noexcept;

    common::xnode_address_t const & address() const noexcept;

    common::xip2_t xip2() const;

protected:
    void swap(xtcash_basic_element & other) noexcept;

    bool operator==(xtcash_basic_element const & other) const noexcept;
};
using xbasic_element_t = xtcash_basic_element;

NS_END3
