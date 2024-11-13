// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"

#include <system_error>

NS_BEG1(tcash)

template <typename T>
class xtcash_enable_to_bytes {
public:
    xtcash_enable_to_bytes() = default;
    xtcash_enable_to_bytes(xtcash_enable_to_bytes const &) = default;
    xtcash_enable_to_bytes & operator=(xtcash_enable_to_bytes const &) = default;
    xtcash_enable_to_bytes(xtcash_enable_to_bytes &&) = default;
    xtcash_enable_to_bytes & operator=(xtcash_enable_to_bytes &&) = default;
    virtual ~xtcash_enable_to_bytes() = default;

    virtual xbytes_t to_bytes() const = 0;
    virtual void from_bytes(xbytes_t const & bytes, std::error_code & ec) = 0;
};
template <typename T>
using xenable_to_bytes_t = xtcash_enable_to_bytes<T>;

NS_END1
