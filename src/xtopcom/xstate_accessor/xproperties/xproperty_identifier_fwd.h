// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xstate_accessor/xproperties/xproperty_type.h"

NS_BEG3(tcash, state_accessor, properties)

class xtcash_property_identifier;
using xproperty_identifier_t = xtcash_property_identifier;

class xtcash_typeless_property_identifier;
using xtypeless_property_identifier_t = xtcash_typeless_property_identifier;

//template <xproperty_type_t PropertyTypeV, typename std::enable_if<PropertyTypeV != xproperty_type_t::invalid>::type * = nullptr>
//class xtcash_typed_property_identifier;
//
//template <xproperty_type_t PropertyTypeV>
//using xtyped_property_identifier_t = xtcash_typed_property_identifier<PropertyTypeV>;

NS_END3

