// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xcodec/xmsgpack/xid_codec.hpp"
#include "xbasic/xcodec/xmsgpack/xversion_codec.hpp"
#include "xcommon/xcodec/xmsgpack/xnode_id_codec.hpp"
#include "xcommon/xip.h"
#include "xdata/xcodec/xmsgpack/xelection/xv1/xelection_info_bundle_codec.hpp"
#include "xdata/xcodec/xmsgpack/xelection/xv1/xelection_info_codec.hpp"
#include "xdata/xelection/xv1/xelection_group_result.h"

#include <cstdint>
#include <string>

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

XINLINE_CONSTEXPR std::size_t xv1_election_group_result_field_count{ 8 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_results_index{ 0 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_associated_group_id_index{ 1 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_group_version_index{ 2 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_associated_group_version_index{ 3 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_cluster_version_index{ 4 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_election_committee_version_index{ 5 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_timestamp_index{ 6 };
XINLINE_CONSTEXPR std::size_t xv1_election_group_result_start_time_index{ 7 };

template <>
struct convert<tcash::data::election::v1::xelection_group_result_t> final {
    msgpack::object const & operator()(msgpack::object const & o, tcash::data::election::v1::xelection_group_result_t & result) const {
        if (o.type != msgpack::type::ARRAY) {
            throw msgpack::type_error{};
        }

        if (o.via.array.size == 0) {
            return o;
        }

        auto * array = o.via.array.ptr;
        switch (o.via.array.size - 1) {
            default: {
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_start_time_index: {
                result.start_time(array[xv1_election_group_result_start_time_index].as<tcash::common::xlogic_time_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_timestamp_index: {
                result.timestamp(array[xv1_election_group_result_timestamp_index].as<tcash::common::xlogic_time_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_election_committee_version_index: {
                result.election_committee_version(array[xv1_election_group_result_election_committee_version_index].as<tcash::common::xelection_round_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_cluster_version_index: {
                result.cluster_version(array[xv1_election_group_result_cluster_version_index].as<tcash::common::xelection_round_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_associated_group_version_index: {
                result.associated_group_version(array[xv1_election_group_result_associated_group_version_index].as<tcash::common::xelection_round_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_group_version_index: {
                result.group_version(array[xv1_election_group_result_group_version_index].as<tcash::common::xelection_round_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_associated_group_id_index: {
                result.associated_group_id(array[xv1_election_group_result_associated_group_id_index].as<tcash::common::xgroup_id_t>());
                XATTRIBUTE_FALLTHROUGH;
            }

            case xv1_election_group_result_results_index: {
                result.results(array[xv1_election_group_result_results_index].as<std::map<tcash::common::xslot_id_t, tcash::data::election::v1::xelection_info_bundle_t>>());
                XATTRIBUTE_FALLTHROUGH;
            }
        }

        return o;
    }
};

template <>
struct pack<::tcash::data::election::v1::xelection_group_result_t> {
    template <typename StreamT>
    msgpack::packer<StreamT> & operator()(msgpack::packer<StreamT> & o, tcash::data::election::v1::xelection_group_result_t const & result) const {
        o.pack_array(xv1_election_group_result_field_count);
        o.pack(result.results());
        o.pack(result.associated_group_id());
        o.pack(result.group_version());
        o.pack(result.associated_group_version());
        o.pack(result.cluster_version());
        o.pack(result.election_committee_version());
        o.pack(result.timestamp());
        o.pack(result.start_time());
        return o;
    }
};

template <>
struct object_with_zone<tcash::data::election::v1::xelection_group_result_t> {
    void operator()(msgpack::object::with_zone & o, tcash::data::election::v1::xelection_group_result_t const & result) const {
        o.type = msgpack::type::ARRAY;
        o.via.array.size = xv1_election_group_result_field_count;
        o.via.array.ptr = static_cast<msgpack::object *>(o.zone.allocate_align(sizeof(::msgpack::object) * o.via.array.size));
        o.via.array.ptr[xv1_election_group_result_results_index]                    = msgpack::object{ result.results(),                        o.zone };
        o.via.array.ptr[xv1_election_group_result_associated_group_id_index]        = msgpack::object{ result.associated_group_id(),            o.zone };
        o.via.array.ptr[xv1_election_group_result_group_version_index]              = msgpack::object{ result.group_version(),                  o.zone };
        o.via.array.ptr[xv1_election_group_result_associated_group_version_index]   = msgpack::object{ result.associated_group_version(),       o.zone };
        o.via.array.ptr[xv1_election_group_result_cluster_version_index]            = msgpack::object{ result.cluster_version(),                o.zone };
        o.via.array.ptr[xv1_election_group_result_election_committee_version_index] = msgpack::object{ result.election_committee_version(),     o.zone };
        o.via.array.ptr[xv1_election_group_result_timestamp_index]                  = msgpack::object{ result.timestamp(),                      o.zone };
        o.via.array.ptr[xv1_election_group_result_start_time_index]                 = msgpack::object{ result.start_time(),                     o.zone };
    }
};

NS_END1
}
NS_END1
