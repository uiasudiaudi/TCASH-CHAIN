// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcodec/xdecorators/xmsgpack_decorator.hpp"

#include <exception>

NS_BEG2(tcash, codec)

/**
 * @brief Encoding & decoding wrapper for objects based on MsgPack.
 *
 * @tparam T Target type for encoding & decoding. This type must support MsgPask's pack & unpack.
 */
template <typename T>
struct xtcash_msgpack_codec final
{
    xtcash_msgpack_codec()                                       = delete;
    xtcash_msgpack_codec(xtcash_msgpack_codec const &)             = delete;
    xtcash_msgpack_codec & operator=(xtcash_msgpack_codec const &) = delete;
    xtcash_msgpack_codec(xtcash_msgpack_codec &&)                  = delete;
    xtcash_msgpack_codec & operator=(xtcash_msgpack_codec &&)      = delete;
    ~xtcash_msgpack_codec()                                      = delete;

    static_assert(std::is_default_constructible<T>::value, "object must be default constructible");

    /**
     * @brief Entrypoint for encoding message.
     *
     * @tparam ArgsT            Extra types of arguments for encoding the message.
     * @param message           The message to be encoded.
     * @param args              Extra arguments for encoding the message.
     * @return xbyte_buffer_t   Encode result.
     */
    template <typename ... ArgsT>
    static
    xbyte_buffer_t
    encode(T const & message, ArgsT && ... args) {
        return decorators::xmsgpack_decorator_t<T>::encode(message, std::forward<ArgsT>(args)...);
    }

    /**
     * @brief Entrypoint for decoding the data.
     *
     * @tparam ArgsT    Extra types of arguments for decoding the data.
     * @param in        Serialized input data.
     * @param args      Extra arguments for decoding the data.
     * @return decltype(decorators::xmsgpack_decorator_t<T>::decode(in, std::forward<ArgsT>(args)...))
     */
    template <typename ... ArgsT>
    static
    auto
    decode(xbyte_buffer_t const & in, ArgsT && ... args) -> decltype(decorators::xmsgpack_decorator_t<T>::decode(in, std::forward<ArgsT>(args)...)) {
        return decorators::xmsgpack_decorator_t<T>::decode(in, std::forward<ArgsT>(args)...);
    }

    static T decode(xbyte_buffer_t const & in, std::error_code & ec) {
        return decorators::xmsgpack_decorator_t<T>::decode(in, ec);
    }
};

template <typename T>
using xmsgpack_codec_t = xtcash_msgpack_codec<T>;

/**
 * @brief Simple wrapper of xmsgpack_codec_t::encode.
 *
 * @tparam T                Message type.
 * @tparam ArgsT            Extra types of arguments for encoding.
 * @param message           Message to be encoded.
 * @param args              Extra arguments for encoding.
 * @return xbyte_buffer_t   Encode result.
 */
template <typename T, typename ... ArgsT>
xbyte_buffer_t
msgpack_encode(T const & message, ArgsT && ... args) {
    try {
        return xmsgpack_codec_t<T>::encode(message, std::forward<ArgsT>(args)...);
    } catch (std::exception const &) {
        // TODO: log
        assert(false);
        return {};
    } catch (...) {
        // TODO: log
        assert(false);
        return{};
    }
}

/**
 * @brief Simple wrapper of xmsgpack_codec_t::decode.
 *
 * @tparam T                Message type.
 * @tparam ArgsT            Extra types of arguments for decoding.
 * @param bytes_message     The input serialzied bytes data.
 * @param args              Extra arguments for decoding.
 * @return T                Message type.
 */
template <typename T, typename ... ArgsT>
T
msgpack_decode(xbyte_buffer_t const & bytes_message, ArgsT && ... args) {
    try {
        return xmsgpack_codec_t<T>::decode(bytes_message, std::forward<ArgsT>(args)...);
    } catch (std::exception const &) {
        // TODO: log
        assert(false);
        return {};
    } catch (...) {
        // TODO: log
        assert(false);
        return {};
    }
}

NS_END2
