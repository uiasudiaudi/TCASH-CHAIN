// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <system_error>

namespace tcash {
namespace blockmaker {
namespace error {

enum class xenum_errc {
    ok,
    blockmaker_load_block_fail,
    blockmaker_cert_block_changed,
    blockmaker_connect_block_behind,
    blockmaker_drand_block_invalid,
    blockmaker_make_table_state_fail,
    blockmaker_create_statectx_fail,
    blockmaker_make_unit_fail,
    blockmaker_relayblock_unconnect,
    blockmaker_load_unitstate,
    blockmaker_property_invalid,
    blockmaker_no_valid_txs,
};
using xerrc_t = xenum_errc;

std::error_code make_error_code(xerrc_t errc) noexcept;
std::error_condition make_error_condition(xerrc_t errc) noexcept;

std::error_category const & blockmaker_category();

}
}
}

namespace std {

#if !defined(XCXX14)

template <>
struct hash<tcash::blockmaker::error::xerrc_t> final {
    size_t operator()(tcash::blockmaker::error::xerrc_t errc) const noexcept;
};

#endif

template <>
struct is_error_code_enum<tcash::blockmaker::error::xerrc_t> : std::true_type {
};

template <>
struct is_error_condition_enum<tcash::blockmaker::error::xerrc_t> : std::true_type {
};

}
