// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xcodec/xmsgpack/xnode_id_codec.hpp"
#include "xdata/xelection/xv1/xelection_info_bundle.h"

#include <cstdint>

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

XINLINE_CONSTEXPR std::size_t xv1_election_info_bundle_field_count{ 2 };
XINLINE_CONSTEXPR std::size_t xv1_election_info_bundle_node_id_index{ 0 };
XINLINE_CONSTEXPR std::size_t xv1_election_info_bundle_election_info_index{ 1 };

template <>
struct convert<tcash::data::election::v1::xelection_info_bundle_t> final {
    msgpack::object const & operator()(msgpack::object const & o, tcash::data::election::v1::xelection_info_bundle_t & info_bundle) const {
        if (o.type != msgpack::type::ARRAY) {
            throw msgpack::type_error{};
        }

        if (o.via.array.size == 0) {
            return o;
        }

        switch (o.via.array.size - 1) {
            default: {
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_info_bundle_election_info_index: {
                info_bundle.election_info(o.via.array.ptr[xv1_election_info_bundle_election_info_index].as<tcash::data::election::v1::xelection_info_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_info_bundle_node_id_index: {
                info_bundle.node_id(o.via.array.ptr[xv1_election_info_bundle_node_id_index].as<tcash::common::xnode_id_t>());
                XATTRIBUTE_FALLTHROUGH;
            }
        }

        return o;
    }
};

template <>
struct pack<::tcash::data::election::v1::xelection_info_bundle_t> {
    template <typename StreamT>
    msgpack::packer<StreamT> & operator()(msgpack::packer<StreamT> & o, tcash::data::election::v1::xelection_info_bundle_t const & info_bundle) const {
        o.pack_array(xv1_election_info_bundle_field_count);
        o.pack(info_bundle.node_id());
        o.pack(info_bundle.election_info());

        return o;
    }
};

template <>
struct object_with_zone<::tcash::data::election::v1::xelection_info_bundle_t> {
    void operator()(msgpack::object::with_zone & o, tcash::data::election::v1::xelection_info_bundle_t const & info_bundle) const {
        o.type = msgpack::type::ARRAY;
        o.via.array.size = xv1_election_info_bundle_field_count;
        o.via.array.ptr = static_cast<msgpack::object *>(o.zone.allocate_align(sizeof(::msgpack::object) * o.via.array.size));
        o.via.array.ptr[xv1_election_info_bundle_node_id_index]       = msgpack::object{ info_bundle.node_id(),       o.zone };
        o.via.array.ptr[xv1_election_info_bundle_election_info_index] = msgpack::object{ info_bundle.election_info(), o.zone };
    }
};

NS_END1
}
NS_END1
