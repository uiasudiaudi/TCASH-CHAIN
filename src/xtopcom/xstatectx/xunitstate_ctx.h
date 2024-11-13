﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include "xbasic/xmemory.hpp"
#include "xvledger/xvstate.h"
#include "xvledger/xvblock.h"
#include "xdata/xunit_bstate.h"

NS_BEG2(tcash, statectx)

// unit state context is a wrap unitstate 
class xunitstate_ctx_t {
 public:
    xunitstate_ctx_t(data::xunitstate_ptr_t const& unitstate, base::xaccount_index_t const& accoutindex);
    xunitstate_ctx_t(data::xaccountstate_ptr_t const& accountstate) 
    : m_accountstate(accountstate) {}    

 public:
    void    set_unit(base::xvblock_ptr_t const& unit) {m_unit = unit;}
    void    set_unitstate_bin(std::string const& bin) {m_bstate_bin = bin;}
 public:
    // common::xaccount_address_t const&   get_address() const {return m_cur_unitstate->account_address();}
    const data::xunitstate_ptr_t &      get_unitstate() const {return m_accountstate->get_unitstate();}
    std::string const&                  get_unit_hash() const{return m_accountstate->get_unit_hash();}
    data::xaccountstate_ptr_t const&    get_accoutstate() const {return m_accountstate;}
    base::xvblock_ptr_t const&          get_unit() const{return m_unit;}
    std::string const&                  get_unitstate_bin() const{return m_bstate_bin;}

 private: 
    data::xaccountstate_ptr_t           m_accountstate{nullptr};
    base::xvblock_ptr_t                 m_unit{nullptr};
    std::string                         m_bstate_bin;
};
using xunitstate_ctx_ptr_t = std::shared_ptr<xunitstate_ctx_t>;

NS_END2
