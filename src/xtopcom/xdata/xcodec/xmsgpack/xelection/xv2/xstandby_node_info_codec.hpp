﻿// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xcodec/xmsgpack/xcrypto_key_codec.hpp"
#include "xcommon/xcodec/xmsgpack/xnode_type_codec.hpp"
#include "xcommon/xcodec/xmsgpack/xrole_type_codec.hpp"
#include "xdata/xelection/xv2/xstandby_node_info.h"

#include <msgpack.hpp>

#if defined(DEBUG)
#   include <cinttypes>
#endif
#include <cstdint>

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
NS_BEG1(adaptor)

XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_field_count{6};
XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_public_key_index{0};
XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_stake_index{1};
XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_program_version_index{2};
XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_genesis_index{3};
XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_miner_type_index{4};
XINLINE_CONSTEXPR std::size_t xv2_standby_node_info_credit_scores_index{5};

template <>
struct convert<tcash::data::election::v2::xstandby_node_info_t> final {
    msgpack::object const & operator()(msgpack::object const & o, tcash::data::election::v2::xstandby_node_info_t & node_info) const {
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

        case xv2_standby_node_info_credit_scores_index: {
            node_info.raw_credit_scores = o.via.array.ptr[xv2_standby_node_info_credit_scores_index].as<std::map<tcash::common::xnode_type_t, uint64_t>>();
#if defined(DEBUG)            
            for (auto const & raw_credit_score_info : node_info.raw_credit_scores) {
                xdbg("standby_node_info_codec unpack: %s:%" PRIu64, tcash::common::to_string(raw_credit_score_info.first).c_str(), raw_credit_score_info.second);
            }
#endif            
            XATTRIBUTE_FALLTHROUGH;
        }

        case xv2_standby_node_info_miner_type_index: {
            node_info.miner_type = o.via.array.ptr[xv2_standby_node_info_miner_type_index].as<tcash::common::xminer_type_t>();
            XATTRIBUTE_FALLTHROUGH;
        }

        case xv2_standby_node_info_genesis_index: {
            node_info.genesis = o.via.array.ptr[xv2_standby_node_info_genesis_index].as<bool>();
            XATTRIBUTE_FALLTHROUGH;
        }

        case xv2_standby_node_info_program_version_index: {
            node_info.program_version = o.via.array.ptr[xv2_standby_node_info_program_version_index].as<std::string>();
            XATTRIBUTE_FALLTHROUGH;
        }

        case xv2_standby_node_info_stake_index: {
            node_info.stake_container = o.via.array.ptr[xv2_standby_node_info_stake_index].as<std::map<tcash::common::xnode_type_t, uint64_t>>();
            XATTRIBUTE_FALLTHROUGH;
        }
        

        case xv2_standby_node_info_public_key_index: {
            node_info.consensus_public_key = o.via.array.ptr[xv2_standby_node_info_public_key_index].as<tcash::xpublic_key_t>();
            break;
        }
        }

        return o;
    }
};

template <>
struct pack<::tcash::data::election::v2::xstandby_node_info_t> {
    template <typename StreamT>
    msgpack::packer<StreamT> & operator()(msgpack::packer<StreamT> & o, tcash::data::election::v2::xstandby_node_info_t const & node_info) const {
        o.pack_array(xv2_standby_node_info_field_count);
        o.pack(node_info.consensus_public_key);
        o.pack(node_info.stake_container);
        o.pack(node_info.program_version);
        o.pack(node_info.genesis);
        o.pack(node_info.miner_type);
        o.pack(node_info.raw_credit_scores);

#if defined(DEBUG)
        for (auto const & raw_credit_score_info : node_info.raw_credit_scores) {
            xdbg("standby_node_info_codec pack: %s:%" PRIu64, tcash::common::to_string(raw_credit_score_info.first).c_str(), raw_credit_score_info.second);
        }
#endif

        return o;
    }
};

template <>
struct object_with_zone<::tcash::data::election::v2::xstandby_node_info_t> {
    void operator()(msgpack::object::with_zone & o, tcash::data::election::v2::xstandby_node_info_t const & node_info) const {
        o.type = msgpack::type::ARRAY;
        o.via.array.size = xv2_standby_node_info_field_count;
        o.via.array.ptr = static_cast<msgpack::object *>(o.zone.allocate_align(sizeof(::msgpack::object) * o.via.array.size));
        o.via.array.ptr[xv2_standby_node_info_public_key_index] = msgpack::object{node_info.consensus_public_key, o.zone};
        o.via.array.ptr[xv2_standby_node_info_stake_index] = msgpack::object{node_info.stake_container, o.zone};
        o.via.array.ptr[xv2_standby_node_info_program_version_index] = msgpack::object{node_info.program_version, o.zone};
        o.via.array.ptr[xv2_standby_node_info_genesis_index] = msgpack::object{node_info.genesis, o.zone};
        o.via.array.ptr[xv2_standby_node_info_miner_type_index] = msgpack::object{node_info.miner_type, o.zone};
        o.via.array.ptr[xv2_standby_node_info_credit_scores_index] = msgpack::object{node_info.raw_credit_scores, o.zone};
    }
};

NS_END1
}
NS_END1
