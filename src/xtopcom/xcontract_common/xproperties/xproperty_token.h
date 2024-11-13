// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xsymbol.h"
#include "xcommon/xtoken.h"
#include "xcontract_common/xcontract_fwd.h"
#include "xcontract_common/xproperties/xbasic_property.h"

#include <cstdint>
#include <functional>
#include <string>
#include <system_error>

NS_BEG3(tcash, contract_common, properties)

//class xtcash_token_safe {
//public:
//    // static bool transfer_safe_rule(evm_common::u256 amount) noexcept;
//    // static void set_max_safe_token(uint64_t amount) noexcept;
//
//private:
//   constexpr static uint64_t MAX_SAFE_TOKEN = 200 * 1e8 * 1e6; //utcash
//};
//using xtoken_safe_t = xtcash_token_safe;

// token object
class xtcash_token_property: public xbasic_property_t {
    common::xsymbol_t m_symbol{common::SYMBOL_tcash_TOKEN};

public:
    xtcash_token_property() = default;
    xtcash_token_property(xtcash_token_property const&) = delete;
    xtcash_token_property& operator=(xtcash_token_property const&) = delete;
    xtcash_token_property(xtcash_token_property&&) = default;
    xtcash_token_property& operator=(xtcash_token_property&&) = default;
    ~xtcash_token_property() override = default;

    explicit xtcash_token_property(std::string const& name, common::xsymbol_t symbol, contract_common::xcontract_face_t* contract);
    explicit xtcash_token_property(common::xsymbol_t symbol, contract_common::xcontract_face_t * contract);
    explicit xtcash_token_property(std::string const & name, contract_common::xcontract_face_t * contract);
    explicit xtcash_token_property(contract_common::xcontract_face_t * contract);

    evm_common::u256 amount() const;
    common::xtoken_t withdraw(evm_common::u256 amount);
    void deposit(common::xtoken_t tokens);
    common::xsymbol_t const & symbol() const noexcept;
};
using xtoken_property_t = xtcash_token_property;

NS_END3

NS_BEG1(std)

template <>
struct hash<tcash::contract_common::properties::xtoken_property_t> {
    size_t operator()(tcash::contract_common::properties::xtoken_property_t const & token_property) const;
};

NS_END1
