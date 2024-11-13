// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xutl.h"
#include "xcodec/xmsgpack_codec.hpp"
#include "xvm/xcontract/xcontract_base.h"

#include <string>

NS_BEG3(tcash, xvm, serialization)

template <typename T>
struct xtcash_msgpack final {
    static
    T
    deserialize_from_string_prop(xcontract::xcontract_base const & contract, std::string const & property_name) {
        try {
            auto string_value = contract.STRING_GET(property_name);
            if (string_value.empty()) {
                return T{};
            }

            return codec::msgpack_decode<T>({std::begin(string_value), std::end(string_value)});
        } catch (tcash::error::xtcash_error_t const & eh) {
            xwarn("[xvm] deserialize %s failed. error category: %s: msg: %s", property_name.c_str(), eh.code().category().name(), eh.what());
            throw;
        }
    }

    static
    T
    deserialize_from_string_prop(xcontract::xcontract_base const & contract,
                                 std::string const & another_contract_address,
                                 std::string const & property_name) {
        try {
            auto string_value = contract.QUERY(xcontract::enum_type_t::string, property_name, "", another_contract_address);
            if (string_value.empty()) {
                return T{};
            }

            return codec::msgpack_decode<T>({std::begin(string_value), std::end(string_value)});
        } catch (tcash::error::xtcash_error_t const & eh) {
            xwarn("[xvm] deserialize %s failed. category: %s; msg: %s", property_name.c_str(), eh.code().category().name(), eh.what());
            throw;
        }
    }

    static
    void
    serialize_to_string_prop(xcontract::xcontract_base & contract, std::string const & property_name, T const & object) {
        auto bytes = codec::msgpack_encode(object);
        std::string obj_str{ std::begin(bytes), std::end(bytes) };
        uint256_t hash = utl::xsha2_256_t::digest((const char*)obj_str.data(), obj_str.size());
        xinfo("serialize_to_string_prop: %s, %s, %u, %s", typeid(contract).name(), property_name.c_str(), obj_str.size(), data::to_hex_str(hash).c_str());
        contract.STRING_SET(property_name, obj_str);
#if defined DEBUG
        auto base64str = base::xstring_utl::base64_encode(bytes.data(), static_cast<std::uint32_t>(bytes.size()));
        xdbg("[serialization] property %s hash %s", property_name.c_str(), base64str.c_str());
#endif
    }

    static
    std::string
    serialize_to_string_prop(T const & object) {
        auto bytes = codec::msgpack_encode(object);
        return { std::begin(bytes), std::end(bytes) };
    }

    static
    T
    deserialize_from_string_prop(std::string const & property_value) {
        return codec::msgpack_decode<T>({ std::begin(property_value), std::end(property_value) });
    }
};

template <typename T>
using xmsgpack_t = xtcash_msgpack<T>;

NS_END3
