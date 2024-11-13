#pragma once

#include "xbase/xns_macro.h"

#include <system_error>

NS_BEG2(tcash, db_export)

enum class xtcash_errc {
    ok,
    invalid_table_address,
    table_block_not_found,
    table_state_not_found,
    account_data_not_found,
    unknown_token,
    unit_state_not_found,
};
using xerrc_t = xtcash_errc;

std::error_code make_error_code(xerrc_t ec) noexcept;

std::error_category const & db_export_category();

NS_END2

NS_BEG1(std)

template <>
struct is_error_code_enum<tcash::db_export::xerrc_t> : true_type {};

NS_END1
