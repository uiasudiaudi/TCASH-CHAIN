// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xid.hpp"

#include <msgpack.hpp>

#include <type_traits>

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

//template <typename TagT, typename IdT>
//struct as<tcash::xnullable_id_t<TagT, IdT>, typename std::enable_if<msgpack::has_as<IdT>::value>::type>
//{
//    tcash::xnullable_id_t<TagT, IdT>
//    operator()(msgpack::object const& o) const {
//        if (o.is_nil()) {
//            return tcash::xnullable_id_t<TagT, IdT>{};
//        }
//
//        return o.as<IdT>();
//    }
//};
//
//template <typename TagT, typename IdT>
//struct convert<tcash::xnullable_id_t<TagT, IdT>> final
//{
//    msgpack::object const &
//    operator()(msgpack::object const & o, tcash::xnullable_id_t<TagT, IdT> & v) const {
//        if (o.is_nil()) {
//            v = tcash::xnullable_id_t<TagT, IdT>{};
//        } else {
//            IdT t;
//            msgpack::adaptor::convert<IdT>()(o, t);
//            v = tcash::xnullable_id_t<TagT, IdT>{ t };
//        }
//
//        return o;
//    }
//};
//
//template <typename TagT, typename IdT>
//struct pack<tcash::xnullable_id_t<TagT, IdT>>
//{
//    template <typename Stream>
//    msgpack::packer<Stream> &
//    operator()(msgpack::packer<Stream> & o, tcash::xnullable_id_t<TagT, IdT> const & message) const {
//        if (message.has_value()) {
//            o.pack(message.value());
//        } else {
//            o.pack_nil();
//        }
//
//        return o;
//    }
//};
//
//template <typename TagT, typename IdT>
//struct object<tcash::xnullable_id_t<TagT, IdT>>
//{
//    void
//    operator()(msgpack::object & o, tcash::xnullable_id_t<TagT, IdT> const & message) const {
//        if (message.has_value()) {
//            msgpack::adaptor::object<IdT>()(o, message.value());
//        } else {
//            o.type = msgpack::type::NIL;
//        }
//    }
//};
//
//template <typename TagT, typename IdT>
//struct object_with_zone<tcash::xnullable_id_t<TagT, IdT>>
//{
//    void
//    operator()(msgpack::object::with_zone & o, tcash::xnullable_id_t<TagT, IdT> const & message) const {
//        if (message.has_value()) {
//            msgpack::adaptor::object_with_zone<IdT>()(o, message.value());
//        } else {
//            o.type = msgpack::type::NIL;
//        }
//    }
//};

template <typename TagT, typename IdT, IdT MinValue, IdT MaxValue>
struct as<tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue>, typename std::enable_if<msgpack::has_as<IdT>::value>::type> {
    tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue> operator()(msgpack::object const & o) const {
        return o.as<IdT>();
    }
};

template <typename TagT, typename IdT, IdT MinValue, IdT MaxValue>
struct convert<tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue>> final {
    msgpack::object const & operator()(msgpack::object const & o, tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue> & v) const {
        if (!o.is_nil()) {
            IdT t;
            msgpack::adaptor::convert<IdT>()(o, t);
            v = tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue>{ t };
        }

        return o;
    }
};

template <typename TagT, typename IdT, IdT MinValue, IdT MaxValue>
struct pack<tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue>> {
    template <typename Stream>
    msgpack::packer<Stream> & operator()(msgpack::packer<Stream> & o, tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue> const & message) const {
        o.pack(message.value());
        return o;
    }
};

template <typename TagT, typename IdT, IdT MinValue, IdT MaxValue>
struct object<tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue>> {
    void operator()(msgpack::object & o, tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue> const & message) const {
        msgpack::adaptor::object<IdT>()(o, message.value());
    }
};

template <typename TagT, typename IdT, IdT MinValue, IdT MaxValue>
struct object_with_zone<tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue>> {
    void operator()(msgpack::object::with_zone & o, tcash::xsimple_id_t<TagT, IdT, MinValue, MaxValue> const & message) const {
        msgpack::adaptor::object_with_zone<IdT>()(o, message.value());
    }
};

NS_END1
}
NS_END1
