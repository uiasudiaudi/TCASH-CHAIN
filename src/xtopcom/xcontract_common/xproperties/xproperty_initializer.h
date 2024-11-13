// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xmemory.hpp"
#include "xcontract_common/xproperties/xbasic_property.h"

#include <vector>

NS_BEG3(tcash, contract_common, properties)

class xtcash_property_initializer {
    std::vector<observer_ptr<xbasic_property_t>> m_properties;

public:
    xtcash_property_initializer() = default;
    xtcash_property_initializer(xtcash_property_initializer const &) = delete;
    xtcash_property_initializer & operator=(xtcash_property_initializer const &) = delete;
    xtcash_property_initializer(xtcash_property_initializer &&) = default;
    xtcash_property_initializer & operator=(xtcash_property_initializer &&) = default;
    ~xtcash_property_initializer() = default;

    void register_property(observer_ptr<xbasic_property_t> const & property);
    void initialize() const;
};
using xproperty_initializer_t = xtcash_property_initializer;

NS_END3
