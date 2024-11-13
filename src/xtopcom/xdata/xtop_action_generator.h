// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xmemory.hpp"
#include "xdata/xcons_transaction.h"
#include "xdata/xtcash_action.h"

#include <memory>
#include <vector>

NS_BEG2(tcash, contract_runtime)

class xtcash_action_generator {
public:
    static std::unique_ptr<data::xbasic_tcash_action_t const> generate(xobject_ptr_t<data::xcons_transaction_t> const & tx);
    static std::unique_ptr<data::xbasic_tcash_action_t const> generate(xobject_ptr_t<data::xcons_transaction_t> const & tx, std::error_code & ec);
    static std::vector<std::unique_ptr<data::xbasic_tcash_action_t const>> generate(std::vector<xobject_ptr_t<data::xcons_transaction_t>> const & txs);
    static std::vector<std::unique_ptr<data::xbasic_tcash_action_t const>> generate(std::vector<xobject_ptr_t<data::xcons_transaction_t>> const & txs, std::error_code & ec);
};
using xaction_generator_t = xtcash_action_generator;

NS_END2
