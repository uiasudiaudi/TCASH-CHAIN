﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include "xbasic/xmemory.hpp"
#include "xvledger/xvstate.h"
#include "xvledger/xvblock.h"
#include "xdata/xtable_bstate.h"
#include "xstate_mpt/xstate_mpt.h"

NS_BEG2(tcash, statectx)

// table world state context
// the table state is a temp state
class xtablestate_ctx_t {
 public:
    xtablestate_ctx_t(const data::xtablestate_ptr_t & table_state, std::shared_ptr<state_mpt::xstate_mpt_t> const& state_mpt);
 public:
    std::string get_table_address() const {
        return m_table_state->account_address().to_string();
    }
    base::xtable_shortid_t              get_tableid() const {return m_table_state->get_bstate()->get_short_table_id();}
    uint64_t                            get_table_commit_height() const;
    const data::xtablestate_ptr_t &     get_table_state() const {return m_table_state;}
    std::shared_ptr<state_mpt::xstate_mpt_t> const&    get_state_mpt() const {return m_state_mpt;}
 private:
    data::xtablestate_ptr_t             m_table_state{nullptr};
    std::shared_ptr<state_mpt::xstate_mpt_t>  m_state_mpt{nullptr};
};
using xtablestate_ctx_ptr_t = std::shared_ptr<xtablestate_ctx_t>;


NS_END2
