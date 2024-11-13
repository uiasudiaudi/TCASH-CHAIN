// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xdata/xtcash_action_type.h"

NS_BEG2(tcash, data)

template <xtcash_action_type_t ActionTypeV>
class xtcash_consensus_action;

template <xtcash_action_type_t ActionTypeV>
using xconsensus_action_t = xtcash_consensus_action<ActionTypeV>;

using xsystem_consensus_action_t = xconsensus_action_t<xtcash_action_type_t::system>;
using xuser_consensus_action_t = xconsensus_action_t<xtcash_action_type_t::user>;
using xevm_consensus_action_t = xconsensus_action_t<xtcash_action_type_t::evm>;

NS_END2
