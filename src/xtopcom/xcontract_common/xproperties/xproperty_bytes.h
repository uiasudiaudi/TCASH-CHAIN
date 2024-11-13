// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcontract_common/xproperties/xbasic_property.h"
#include "xcontract_common/xcontract_face.h"

#include <string>

NS_BEG3(tcash, contract_common, properties)

class xtcash_bytes_property: public xbasic_property_t {
public:
    xtcash_bytes_property() = default;
    xtcash_bytes_property(xtcash_bytes_property const&) = delete;
    xtcash_bytes_property& operator=(xtcash_bytes_property const&) = delete;
    xtcash_bytes_property(xtcash_bytes_property&&) = default;
    xtcash_bytes_property& operator=(xtcash_bytes_property&&) = default;
    ~xtcash_bytes_property() = default;

    explicit xtcash_bytes_property(std::string const & name, xcontract_face_t * contract);
    explicit xtcash_bytes_property(std::string const & name, std::unique_ptr<xcontract_state_t> state_owned);

    void set(xbytes_t const & value);
    void clear();
    xbytes_t value() const;
    size_t size() const;
    bool empty() const;
    // xbytes_t value(common::xaccount_address_t const & contract) const;
};

using xbytes_property_t = xtcash_bytes_property;

NS_END3
