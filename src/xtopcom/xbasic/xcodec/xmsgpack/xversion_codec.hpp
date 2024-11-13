// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xversion.hpp"

#include <msgpack.hpp>

#include <type_traits>

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

template <typename TagT, typename ValueT>
struct as<tcash::xepoch_t<TagT, ValueT>, typename std::enable_if<msgpack::has_as<ValueT>::value>::type>
{
    tcash::xepoch_t<TagT, ValueT>
    operator()(msgpack::object const & o) const {
        if (o.is_nil()) {
            return tcash::xepoch_t<TagT, ValueT>{};
        }

        return tcash::xepoch_t<TagT, ValueT>{ o.as<ValueT>() };
    }
};

template <typename TagT, typename ValueT>
struct convert<tcash::xepoch_t<TagT, ValueT>> final
{
    msgpack::object const &
    operator()(msgpack::object const & o, tcash::xepoch_t<TagT, ValueT> & v) const {
        if (o.is_nil()) {
            v = tcash::xepoch_t<TagT, ValueT>{};
        } else {
            ValueT t;
            msgpack::adaptor::convert<ValueT>{}(o, t);
            v = tcash::xepoch_t<TagT, ValueT>{ t };
        }

        return o;
    }
};

template <typename TagT, typename ValueT>
struct pack<tcash::xepoch_t<TagT, ValueT>>
{
    template <typename Stream>
    msgpack::packer<Stream> &
    operator()(msgpack::packer<Stream> & o, tcash::xepoch_t<TagT, ValueT> const & message) const {
        if (message.has_value()) {
            o.pack(message.value());
        } else {
            o.pack_nil();
        }

        return o;
    }
};

template <typename TagT, typename ValueT>
struct object<tcash::xepoch_t<TagT, ValueT>>
{
    void
    operator()(msgpack::object & o, tcash::xepoch_t<TagT, ValueT> const & message) const {
        if (message.has_value()) {
            msgpack::adaptor::object<ValueT>{}(o, message.value());
        } else {
            o.type = msgpack::type::NIL;
        }
    }
};

template <typename TagT, typename ValueT>
struct object_with_zone<tcash::xepoch_t<TagT, ValueT>>
{
    void
    operator()(msgpack::object::with_zone & o, tcash::xepoch_t<TagT, ValueT> const & message) const {
        if (message.has_value()) {
            msgpack::adaptor::object_with_zone<ValueT>()(o, message.value());
        } else {
            o.type = msgpack::type::NIL;
        }
    }
};

NS_END1
}
NS_END1
