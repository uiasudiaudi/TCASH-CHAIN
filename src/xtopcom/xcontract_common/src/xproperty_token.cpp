// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xcontract_common/xproperties/xproperty_token.h"

#include "xcontract_common/xerror/xerror.h"
#include "xstate_accessor/xproperties/xproperty_type.h"
#include "xcontract_common/xproperties/xproperty_access_control.h"
#include "xcontract_common/xbasic_contract.h"
#include "xcommon/common_data.h"

NS_BEG3(tcash, contract_common, properties)

//bool xtcash_token_safe::transfer_safe_rule(evm_common::u256 amount) noexcept {
//    return amount < MAX_SAFE_TOKEN;
//}

xtcash_token_property::xtcash_token_property(std::string const & name, common::xsymbol_t symbol, contract_common::xcontract_face_t * contract)
  : xbasic_property_t{name, state_accessor::properties::xproperty_type_t::token, make_observer(contract)}, m_symbol{std::move(symbol)} {
}

 xtcash_token_property::xtcash_token_property(std::string const& name, contract_common::xcontract_face_t* contract)
  : xtcash_token_property{name, common::SYMBOL_tcash_TOKEN, contract} {
}

xtcash_token_property::xtcash_token_property(common::xsymbol_t symbol, contract_common::xcontract_face_t * contract)
  : xtcash_token_property{data::XPROPERTY_BALANCE_AVAILABLE, std::move(symbol), contract} {
}

xtcash_token_property::xtcash_token_property(contract_common::xcontract_face_t * contract)
  : xtcash_token_property{data::XPROPERTY_BALANCE_AVAILABLE, common::SYMBOL_tcash_TOKEN, contract} {
}

evm_common::u256 xtcash_token_property::amount() const {
    assert(associated_state() != nullptr);
    return associated_state()->balance(id());
}

common::xtoken_t xtcash_token_property::withdraw(evm_common::u256 amount) {
    xproperty_utl_t::property_assert(this->amount() > amount,  error::xerrc_t::token_not_enough, "[xtcash_token_property::withdraw]withdraw amount overflow, amount: " + evm_common::toBigEndianString(amount));
    return associated_state()->withdraw(id(), m_symbol, amount);
}

void xtcash_token_property::deposit(common::xtoken_t tokens) {
    if (symbol() != tokens.symbol()) {
        tcash::error::throw_error(contract_common::error::xerrc_t::token_symbol_not_matched);
    }

    // xproperty_utl_t::property_assert(xtoken_safe_t::transfer_safe_rule(tokens.amount()),  error::xerrc_t::token_not_enough, "[xtcash_token_property::deposit]deposit amount overflow, amount: " + evm_common::toBigEndianString(tokens.amount()));
    associated_state()->deposit(id(), std::move(tokens));
}

common::xsymbol_t const & xtcash_token_property::symbol() const noexcept {
    return m_symbol;
}

NS_END3

NS_BEG1(std)

size_t hash<tcash::contract_common::properties::xtoken_property_t>::operator()(tcash::contract_common::properties::xtoken_property_t const & token_property) const {
    return std::hash<tcash::common::xsymbol_t>{}(token_property.symbol());
}

NS_END1
