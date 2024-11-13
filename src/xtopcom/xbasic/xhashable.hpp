// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <cstdint>

NS_BEG1(tcash)

template <typename T, typename HashT>
class xtcash_hashable
{
public:
    xtcash_hashable()                                  = default;
    xtcash_hashable(xtcash_hashable const &)             = default;
    xtcash_hashable & operator=(xtcash_hashable const &) = default;
    xtcash_hashable(xtcash_hashable &&)                  = default;
    xtcash_hashable & operator=(xtcash_hashable &&)      = default;
    virtual ~xtcash_hashable()                         = default;

    using hash_result_type = HashT;

    virtual
    hash_result_type
    hash() const = 0;
};

template <typename T, typename HashT = std::uint64_t>
using xhashable_t = xtcash_hashable<T, HashT>;

NS_END1
