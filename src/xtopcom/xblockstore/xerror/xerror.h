// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <system_error>

namespace tcash {
namespace store {
namespace error {

enum class xenum_errc {
    ok,
    store_create_genesis_cb_not_register,
    store_load_block_fail,
};
using xerrc_t = xenum_errc;

std::error_code make_error_code(xerrc_t errc) noexcept;
std::error_condition make_error_condition(xerrc_t errc) noexcept;

std::error_category const & blockstore_category();

}
}
}

namespace std {

#if !defined(XCXX14)

template <>
struct hash<tcash::store::error::xerrc_t> final {
    size_t operator()(tcash::store::error::xerrc_t errc) const noexcept;
};

#endif

template <>
struct is_error_code_enum<tcash::store::error::xerrc_t> : std::true_type {
};

template <>
struct is_error_condition_enum<tcash::store::error::xerrc_t> : std::true_type {
};

}
