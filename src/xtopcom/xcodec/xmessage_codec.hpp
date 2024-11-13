// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"

NS_BEG2(tcash, codec)

template <typename DecoratorT>
struct xtcash_message_codec final
{
    xtcash_message_codec()                                       = delete;
    xtcash_message_codec(xtcash_message_codec const &)             = delete;
    xtcash_message_codec & operator=(xtcash_message_codec const &) = delete;
    xtcash_message_codec(xtcash_message_codec &&)                  = delete;
    xtcash_message_codec & operator=(xtcash_message_codec &&)      = delete;
    ~xtcash_message_codec()                                      = delete;

    template <typename ... ArgsT>
    static
    xbyte_buffer_t
    encode(typename DecoratorT::message_type const & data, ArgsT && ... args) {
        return DecoratorT::encode(data, std::forward<ArgsT>(args)...);
    }

    template <typename ... ArgsT>
    static
    auto
    decode(xbyte_buffer_t const & in, ArgsT && ... args) -> decltype(DecoratorT::decode(in, std::forward<ArgsT>(args)...)) {
        return DecoratorT::decode(in, std::forward<ArgsT>(args)...);
    }
};

template <typename DecoratorT>
using xmessage_codec_t = xtcash_message_codec<DecoratorT>;

NS_END2
