// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xcrypto_key.h"

#include <msgpack.hpp>

#include <string>

// MSGPACK_ADD_ENUM(tcash::xcrypto_key_type_t)

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

template <>
struct convert<tcash::xcrypto_key_t<tcash::pub>> final {
    msgpack::object const &
    operator()(msgpack::object const & o, tcash::xcrypto_key_t<tcash::pub> & v) const {
        if (o.is_nil()) {
            v = tcash::xcrypto_key_t<tcash::pub>{};
        } else {
            std::string t;
            msgpack::adaptor::convert<std::string>{}(o, t);
            v = tcash::xcrypto_key_t<tcash::pub>{ t };
        }

        return o;
    }
};

template <>
struct pack<tcash::xcrypto_key_t<tcash::pub>> {
    template <typename StreamT>
    msgpack::packer<StreamT> &
    operator()(msgpack::packer<StreamT> & o, tcash::xcrypto_key_t<tcash::pub> const & message) const {
        if (message.empty()) {
            o.pack_nil();
        } else {
            o.pack(message.to_string());
        }

        return o;
    }
};

//template <>
//struct object<tcash::xcrypto_key_t<tcash::pub>> {
//    void
//    operator()(msgpack::object & o, tcash::xcrypto_key_t<tcash::pub> const & message) const {
//        if (message.empty()) {
//            o.type = msgpack::type::NIL;
//        } else {
//            msgpack::adaptor::object<std::string>{}(o, message.to_string());
//        }
//    }
//};

template <>
struct object_with_zone<tcash::xcrypto_key_t<tcash::pub>>
{
    void
    operator()(msgpack::object::with_zone & o, tcash::xcrypto_key_t<tcash::pub> const & message) const {
        if (message.empty()) {
            o.type = msgpack::type::NIL;
        } else {
            msgpack::adaptor::object_with_zone<std::string>{}(o, message.to_string());
        }
    }
};

NS_END1
}
NS_END1
