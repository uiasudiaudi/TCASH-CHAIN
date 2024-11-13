// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"
#include "xcodec/xmsgpack_codec.hpp"

NS_BEG3(tcash, contract_common, serialization)

template <typename T>
struct xtcash_msgpack final {
    static xbytes_t serialize_to_bytes(T const & object) {
        return codec::msgpack_encode(object);
    }

    static T deserialize_from_bytes(xbytes_t const & value) {
        return codec::msgpack_decode<T>(value);
    }
};

template <typename T>
using xmsgpack_t = xtcash_msgpack<T>;

NS_END3
