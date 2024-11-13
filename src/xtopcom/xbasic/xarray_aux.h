// Copyright (c) 2022-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <array>

NS_BEG1(tcash)

template <typename T, size_t SizeV, size_t ... SVs>
struct xtcash_std_array_aux {
    using type = std::array<typename xtcash_std_array_aux<T, SVs...>::type, SizeV>;
};

template <typename T, size_t SizeV>
struct xtcash_std_array_aux<T, SizeV> {
    using type = std::array<T, SizeV>;
};

template <typename T, size_t ... SVs>
using xstd_array_t = typename xtcash_std_array_aux<T, SVs...>::type;

NS_END1
