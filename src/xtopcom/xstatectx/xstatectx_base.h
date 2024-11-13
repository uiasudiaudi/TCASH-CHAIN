﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include "xbasic/xmemory.hpp"
#include "xvledger/xvstate.h"
#include "xvledger/xvblock.h"
#include "xvledger/xvblockstore.h"
#include "xdata/xunit_bstate.h"
#include "xdata/xtable_bstate.h"
#include "xstatestore/xtablestate_ext.h"

NS_BEG2(tcash, statectx)

class xstatectx_base_t {
 public:
    xstatectx_base_t(base::xvblock_t* prev_block, const statestore::xtablestate_ext_ptr_t & prev_table_state, base::xvblock_t* commit_block, const statestore::xtablestate_ext_ptr_t & commit_table_state, uint64_t clock);

 public:
    static xobject_ptr_t<base::xvbstate_t> create_proposal_bstate(base::xvblock_t* prev_block, base::xvbstate_t* prev_bstate, uint64_t clock);
    static xobject_ptr_t<base::xvbstate_t> create_proposal_unit_bstate(base::xvbstate_t* prev_bstate, std::string const& last_block_hash);
    static data::xaccountstate_ptr_t create_proposal_account_state(base::xaccount_index_t const& account_index, data::xunitstate_ptr_t const& unitstate);
 public:
    data::xunitstate_ptr_t          load_inner_table_commit_unit_state(const common::xaccount_address_t & addr) const;
    xobject_ptr_t<base::xvbstate_t> change_to_proposal_block_state(base::xaccount_index_t const& account_index, base::xvbstate_t* prev_bstate) const;
    bool                            load_account_index(common::xaccount_address_t const& address, base::xaccount_index_t & account_index) const;

 public:
    base::xvblockstore_t*       get_blockstore() const;
    bool                        get_account_index(const statestore::xtablestate_ext_ptr_t & table_state, const std::string & account, base::xaccount_index_t & account_index) const;
 private:
    statestore::xtablestate_ext_ptr_t     m_table_state{nullptr};
    statestore::xtablestate_ext_ptr_t     m_commit_table_state{nullptr};
    data::xvblock_ptr_t m_pre_block;
    data::xvblock_ptr_t m_commit_block;
    uint64_t    m_clock{0};
};


NS_END2
