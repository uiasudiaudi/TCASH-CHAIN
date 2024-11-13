// Copyright (c) 2022 - present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xoptional.hpp"

#include <cstdint>

NS_BEG2(tcash, fork_points)

/// @brief chain fork point type
enum class xtcash_enum_fork_point_type: uint8_t {
    invalid,
    logic_time,
    drand_height,
    block_height
};
using xfork_point_type_t = xtcash_enum_fork_point_type;

/// @brief chain fork point
struct xtcash_fork_point {
    xtcash_fork_point() = default;
    xtcash_fork_point(xfork_point_type_t const type, uint64_t const point, std::string str) : fork_type{type}, point{point}, description{std::move(str)} {
    }

    xfork_point_type_t fork_type{xfork_point_type_t::invalid};
    uint64_t point{std::numeric_limits<uint64_t>::max()};
    std::string description{};
};
using xfork_point_t = xtcash_fork_point;

std::string dump_fork_points();

extern tcash::optional<xfork_point_t> v1_7_0_block_fork_point;
extern tcash::optional<xfork_point_t> v1_7_0_sync_point;
extern tcash::optional<xfork_point_t> v1_9_0_state_fork_point;  // unitstate not include tx related info
extern tcash::optional<xfork_point_t> xbft_msg_upgrade;

extern tcash::optional<xfork_point_t> v10900_upgrade_table_tickets_contract;
extern tcash::optional<xfork_point_t> v10901_table_tickets_reset;
extern tcash::optional<xfork_point_t> v10901_enable_voting;
extern tcash::optional<xfork_point_t> v10902_table_tickets_reset;
extern tcash::optional<xfork_point_t> v10902_enable_voting;
extern tcash::optional<xfork_point_t> v11100_event;
extern tcash::optional<xfork_point_t> v11200_block_fork_point;
extern tcash::optional<xfork_point_t> v11200_sync_big_packet;
extern tcash::optional<xfork_point_t> v11200_fullnode_elect;
extern tcash::optional<xfork_point_t> v11300_evm_v3_fee_update_point;
NS_END2
