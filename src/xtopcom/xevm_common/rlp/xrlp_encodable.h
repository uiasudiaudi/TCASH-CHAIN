// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"
#include "xbasic/xbyte_buffer.h"
// #include "xevm_common/rlp/xrlp_iowriter.h"

#include <system_error>

NS_BEG3(tcash, evm_common, rlp)

template <typename T>
class xtcash_rlp_encodable {
public:
    xtcash_rlp_encodable() = default;
    xtcash_rlp_encodable(xtcash_rlp_encodable const &) = default;
    xtcash_rlp_encodable & operator=(xtcash_rlp_encodable const &) = default;
    xtcash_rlp_encodable(xtcash_rlp_encodable &&) = default;
    xtcash_rlp_encodable & operator=(xtcash_rlp_encodable &&) = default;
    virtual ~xtcash_rlp_encodable() = default;

    virtual void EncodeRLP(xbytes_t & buf, std::error_code & ec) = 0;
};

template <typename T>
using xrlp_encodable_t = xtcash_rlp_encodable<T>;

NS_END3