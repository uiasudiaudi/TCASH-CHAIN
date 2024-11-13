// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"

#include <exception>

NS_BEG2(tcash, codec)

template <typename T>
struct xtcash_bytes_codec final {
    xtcash_bytes_codec() = delete;
    xtcash_bytes_codec(xtcash_bytes_codec const &) = delete;
    xtcash_bytes_codec & operator=(xtcash_bytes_codec const &) = delete;
    xtcash_bytes_codec(xtcash_bytes_codec &&) = delete;
    xtcash_bytes_codec & operator=(xtcash_bytes_codec &&) = delete;
    ~xtcash_bytes_codec() = delete;

    template <typename... ArgsT>
    static xbytes_t encode(T const & input, ArgsT&&... args) {
        return tcash::to_bytes<T>(input, std::forward<ArgsT>(args)...);
    }

    template <typename... ArgsT>
    static T decode(xbytes_t const & bytes, std::error_code & ec, ArgsT && ... args) {
        return tcash::from_bytes<T>(bytes, ec, std::forward<ArgsT>(args)...);
    }
};

template <typename T>
using xbytes_codec_t = xtcash_bytes_codec<T>;

NS_END2
