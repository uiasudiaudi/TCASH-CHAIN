// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xcontract_common/xproperties/xproperty_integer.h"

#include <cassert>

NS_BEG3(tcash, contract_common, properties)

#define DEFINE_INT_PROPERTY(INT_TYPE)   \
    xtcash_##INT_TYPE##_property::xtcash_##INT_TYPE##_property(std::string const & name, xcontract_face_t * contract)                                                               \
        : xbasic_property_t {name, state_accessor::properties::xproperty_type_t::INT_TYPE, make_observer(contract)} {                                                           \
    }                                                                                                                                                                           \
                                                                                                                                                                                \
    void xtcash_##INT_TYPE##_property::set(INT_TYPE##_t const value) {                                                                                                            \
        assert(associated_state() != nullptr);                                                                                                                                  \
        associated_state()->xcontract_state_t::set_property<state_accessor::properties::xproperty_type_t::INT_TYPE>(typeless_id(), value);                                      \
    }                                                                                                                                                                           \
                                                                                                                                                                                \
    void xtcash_##INT_TYPE##_property::clear() {                                                                                                                                  \
        assert(associated_state() != nullptr);                                                                                                                                  \
        associated_state()->clear_property(id());                                                                                                                               \
    }                                                                                                                                                                           \
                                                                                                                                                                                \
    INT_TYPE##_t xtcash_##INT_TYPE##_property::value() const {                                                                                                                    \
        assert(associated_state() != nullptr);                                                                                                                                  \
        return associated_state()->xcontract_state_t::get_property<state_accessor::properties::xproperty_type_t::INT_TYPE>(typeless_id());                                      \
    }

DEFINE_INT_PROPERTY(uint64)
DEFINE_INT_PROPERTY(int64)

#undef DEFINE_INT_PROPERTY

NS_END3
