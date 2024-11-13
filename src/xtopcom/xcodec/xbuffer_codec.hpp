// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <utility>

NS_BEG2(tcash, codec)

template <typename DecoratorT>
struct xtcash_buffer_codec final
{
    xtcash_buffer_codec()                                      = delete;
    xtcash_buffer_codec(xtcash_buffer_codec const &)             = delete;
    xtcash_buffer_codec & operator=(xtcash_buffer_codec const &) = delete;
    xtcash_buffer_codec(xtcash_buffer_codec &&)                  = delete;
    xtcash_buffer_codec & operator=(xtcash_buffer_codec &&)      = delete;
    ~xtcash_buffer_codec()                                     = delete;

    template <typename InputT, typename ... ArgsT>
    static
    auto
    encode(InputT && in, ArgsT && ... args) -> decltype(DecoratorT::encode(std::forward<InputT>(in), std::forward<ArgsT>(args)...)) {
        return DecoratorT::encode(std::forward<InputT>(in), std::forward<ArgsT>(args)...);
    }

    template <typename InputT, typename ... ArgsT>
    static
    auto
    decode(InputT && in, ArgsT && ... args) -> decltype(DecoratorT::decode(std::forward<InputT>(in), std::forward<ArgsT>(args)...)) {
        return DecoratorT::decode(std::forward<InputT>(in), std::forward<ArgsT>(args)...);
    }
};

template <typename DecoratorT>
using xbuffer_codec_t = xtcash_buffer_codec<DecoratorT>;

NS_END2
