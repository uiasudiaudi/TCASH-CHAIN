// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"
#include "xbasic/xerror/xerror.h"
#include "xcodec/xcodec_errc.h"

#include <msgpack.hpp>

#include <system_error>

NS_BEG3(tcash, codec, decorators)

template <typename T>
struct xtcash_msgpack_decorator final
{
    xtcash_msgpack_decorator()                                           = delete;
    xtcash_msgpack_decorator(xtcash_msgpack_decorator const &)             = delete;
    xtcash_msgpack_decorator & operator=(xtcash_msgpack_decorator const &) = delete;
    xtcash_msgpack_decorator(xtcash_msgpack_decorator &&)                  = delete;
    xtcash_msgpack_decorator & operator=(xtcash_msgpack_decorator &&)      = delete;
    ~xtcash_msgpack_decorator()                                          = delete;

    using message_type = T;

    static
    xbyte_buffer_t
    encode(message_type const & message) {
        try {
            msgpack::sbuffer buffer;
            msgpack::pack(buffer, message);

            return { buffer.data(), buffer.data() + buffer.size() };
        } catch (...) {
            tcash::error::throw_error(tcash::codec::xcodec_errc_t::encode_error, "msgpack encode error");
            return {};
        }
    }

    static
    message_type
    decode(xbyte_buffer_t const & in) {
        try {
            auto object_handle = msgpack::unpack(reinterpret_cast<char const *>(in.data()), in.size(), nullptr);
            auto object = object_handle.get();
            return object.as<T>();
        } catch (...) {
            tcash::error::throw_error(tcash::codec::xcodec_errc_t::decode_error, "msgpack decode error");
            return {};
        }
    }

    static message_type decode(xbyte_buffer_t const & in, std::error_code & ec) {
        try {
            auto object_handle = msgpack::unpack(reinterpret_cast<char const *>(in.data()), in.size(), nullptr);
            auto object = object_handle.get();
            return object.as<T>();
        } catch (...) {
            ec = tcash::codec::xcodec_errc_t::decode_error;
            return {};
        }
    }
};

template <typename T>
using xmsgpack_decorator_t = xtcash_msgpack_decorator<T>;

NS_END3
