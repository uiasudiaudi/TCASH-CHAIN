// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xcodec/xmsgpack/xid_codec.hpp"
#include "xdata/xcodec/xmsgpack/xelection_association_result_codec.hpp"
#include "xdata/xelection/xelection_association_result_store.h"

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

XINLINE_CONSTEXPR std::size_t xelection_association_result_store_field_count{ 1 };
XINLINE_CONSTEXPR std::size_t xelection_association_result_store_results_index{ 0 };

template <>
struct convert<tcash::data::election::xelection_association_result_store_t> final
{
    msgpack::object const &
    operator()(msgpack::object const & o,
               tcash::data::election::xelection_association_result_store_t & result) const {
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
            case xelection_association_result_store_results_index: {
                result.results(o.via.array.ptr[xelection_association_result_store_results_index].as<std::map<tcash::common::xcluster_id_t, tcash::data::election::xelection_association_result_t>>());
                break;
            }
        }
        return o;
    }
};

template <>
struct pack<::tcash::data::election::xelection_association_result_store_t>
{
    template <typename StreamT>
    msgpack::packer<StreamT> &
    operator()(msgpack::packer<StreamT> & o,
               tcash::data::election::xelection_association_result_store_t const & result) const {
        o.pack_array(xelection_association_result_store_field_count);
        o.pack(result.results());
        return o;
    }
};

template <>
struct object_with_zone<tcash::data::election::xelection_association_result_store_t>
{
    void
    operator()(msgpack::object::with_zone & o,
               tcash::data::election::xelection_association_result_store_t const & result) const {
        o.type = msgpack::type::ARRAY;
        o.via.array.size = xelection_association_result_store_field_count;
        o.via.array.ptr = static_cast<msgpack::object *>(o.zone.allocate_align(sizeof(::msgpack::object) * o.via.array.size));
        o.via.array.ptr[xelection_association_result_store_results_index] = msgpack::object{ result.results(), o.zone };
    }
};

NS_END1
}
NS_END1
