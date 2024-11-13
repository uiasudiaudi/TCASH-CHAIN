// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xdata/xtcash_action_type.h"

NS_BEG2(tcash, data)

class xtcash_basic_tcash_action;
using xbasic_tcash_action_t = xtcash_basic_tcash_action;

template <xtcash_action_type_t ActionTypeV>
struct xtcash_tcash_action;

template <xtcash_action_type_t ActionTypeV>
using xtcash_action_t = xtcash_tcash_action<ActionTypeV>;

NS_END2
