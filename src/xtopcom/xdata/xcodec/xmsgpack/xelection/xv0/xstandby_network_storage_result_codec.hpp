// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xcodec/xmsgpack/xnode_id_codec.hpp"
#include "xdata/xcodec/xmsgpack/xelection/xv0/xstandby_node_info_codec.hpp"
#include "xdata/xelection/xv0/xstandby_network_storage_result.h"

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

XINLINE_CONSTEXPR std::size_t xlegacy_standby_network_storage_result_field_count{2};
XINLINE_CONSTEXPR std::size_t xlegacy_standby_network_storage_result_mainnet_activated{1};
XINLINE_CONSTEXPR std::size_t xlegacy_standby_network_storage_result_result_index{0};

template <>
struct convert<tcash::data::election::v0::xstandby_network_storage_result_t> final {
    msgpack::object const & operator()(msgpack::object const & o, tcash::data::election::v0::xstandby_network_storage_result_t & result) const {
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

            case xlegacy_standby_network_storage_result_mainnet_activated: {
                result.set_activate_state(o.via.array.ptr[xlegacy_standby_network_storage_result_mainnet_activated].as<bool>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xlegacy_standby_network_storage_result_result_index: {
                result.results(
                    o.via.array.ptr[xlegacy_standby_network_storage_result_result_index].as<std::map<tcash::common::xnode_id_t, tcash::data::election::v0::xstandby_node_info_t>>());
                XATTRIBUTE_FALLTHROUGH;
            }
        }

        return o;
    }
};

template <>
struct pack<::tcash::data::election::v0::xstandby_network_storage_result_t> {
    template <typename StreamT>
    msgpack::packer<StreamT> & operator()(msgpack::packer<StreamT> & o, tcash::data::election::v0::xstandby_network_storage_result_t const & result) const {
        o.pack_array(xlegacy_standby_network_storage_result_field_count);
        o.pack(result.results());
        o.pack(result.activated_state());

        return o;
    }
};

template <>
struct object_with_zone<tcash::data::election::v0::xstandby_network_storage_result_t> {
    void operator()(msgpack::object::with_zone & o, tcash::data::election::v0::xstandby_network_storage_result_t const & result) const {
        o.type = msgpack::type::ARRAY;
        o.via.array.size = xlegacy_standby_network_storage_result_field_count;
        o.via.array.ptr = static_cast<msgpack::object *>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));

        o.via.array.ptr[xlegacy_standby_network_storage_result_result_index] = msgpack::object{result.results(), o.zone};
        o.via.array.ptr[xlegacy_standby_network_storage_result_mainnet_activated] = msgpack::object{result.activated_state(), o.zone};
    }
};

NS_END1
}
NS_END1
