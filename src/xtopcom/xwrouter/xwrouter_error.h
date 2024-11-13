// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"
#include <system_error>

NS_BEG2(tcash, xwrouter)

enum class xtcash_wrouter_error {
    success = 0,
    hop_num_beyond_max,
    not_find_routing_table,
    empty_dst_address,
    routing_table_find_closest_nodes_fail,
    crossing_network_fail,
    multi_send_partial_fail,
    serialized_fail,
};
using xwrouter_error_t = xtcash_wrouter_error;

std::error_code
make_error_code(xwrouter_error_t const errc) noexcept;

std::error_condition
make_error_condition(xwrouter_error_t const errc) noexcept;

std::error_category const &
wrouter_category();

NS_END2

NS_BEG1(std)

template <>
struct is_error_code_enum<tcash::xwrouter::xwrouter_error_t> : std::true_type {};

NS_END1
