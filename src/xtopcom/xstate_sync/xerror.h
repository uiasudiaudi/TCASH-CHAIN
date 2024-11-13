// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <system_error>

namespace tcash {
namespace state_sync {
namespace error {

enum class xenum_errc {
    ok,

    state_sync_cancel,
    state_sync_overtime,
    state_network_invalid,
    state_data_invalid,
    state_hash_mismatch,

    downloader_is_running,
    downloader_param_invalid,

    unknown,
};
using xerrc_t = xenum_errc;

std::error_code make_error_code(xerrc_t errc) noexcept;
std::error_condition make_error_condition(xerrc_t errc) noexcept;

std::error_category const & state_sync_category();

}  // namespace error
}  // namespace state_sync
}  // namespace tcash

namespace std {

#if !defined(XCXX14)

template <>
struct hash<tcash::state_sync::error::xerrc_t> final {
    size_t operator()(tcash::state_sync::error::xerrc_t errc) const noexcept;
};

#endif

template <>
struct is_error_code_enum<tcash::state_sync::error::xerrc_t> : std::true_type {};

template <>
struct is_error_condition_enum<tcash::state_sync::error::xerrc_t> : std::true_type {};

}  // namespace std
