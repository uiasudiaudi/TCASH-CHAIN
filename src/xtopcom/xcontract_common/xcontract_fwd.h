// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

NS_BEG2(tcash, contract_common)

class xtcash_contract_face;
using xcontract_face_t = xtcash_contract_face;

enum class xtcash_contract_type: std::uint8_t;
using xcontract_type_t = xtcash_contract_type;

struct xtcash_contract_metadata;
using xcontract_metadata_t = xtcash_contract_metadata;

class xtcash_basic_contract;
using xbasic_contract_t = xtcash_basic_contract;

class xtcash_stateless_contract_face;
using xstateless_contract_face_t = xtcash_stateless_contract_face;

class xtcash_basic_stateless_contract;
using xbasic_stateless_contract_t = xtcash_basic_stateless_contract;

NS_END2
