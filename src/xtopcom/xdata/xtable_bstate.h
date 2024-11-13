// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include "xbase/xobject_ptr.h"
#include "xvledger/xvstate.h"
#include "xvledger/xaccountindex.h"
#include "xvledger/xreceiptid.h"
#include "xdata/xbstate_ctx.h"
#include "xstatistic/xstatistic.h"

NS_BEG2(tcash, data)

XINLINE_CONSTEXPR char const * XPROPERTY_TABLE_ACCOUNT_INDEX        = "@T0";
XINLINE_CONSTEXPR char const * XPROPERTY_TABLE_RECEIPTID            = "@T2";


// xtable_bstate_t is a wrap of xvbstate_t for table state
class xtable_bstate_t : public xbstate_ctx_t, public xstatistic::xstatistic_obj_face_t {
 public:
    xtable_bstate_t(base::xvbstate_t* bstate);
    xtable_bstate_t(base::xvbstate_t* bstate, base::xvbstate_t* org_bstate);
    ~xtable_bstate_t();

 public:
    static std::string                      get_receiptid_property_name() {return XPROPERTY_TABLE_RECEIPTID;}
    static std::string                      get_receiptid_property_bin(base::xvbstate_t* bstate);
    static base::xreceiptid_state_ptr_t     make_receiptid_from_state(base::xvbstate_t* bstate);

 public:
    bool                    get_account_index(const std::string & account, base::xaccount_index_t & account_index) const;
    std::set<std::string>   get_all_accounts() const;
    std::vector<std::pair<common::xaccount_address_t, base::xaccount_index_t>> all_accounts() const;
    int32_t                 get_account_size() const;
    bool                    find_receiptid_pair(base::xtable_shortid_t sid, base::xreceiptid_pair_t & pair) const;
    uint32_t                get_unconfirm_tx_num() const {return m_cache_receiptid->get_unconfirm_tx_num();}
    const base::xreceiptid_state_ptr_t & get_receiptid_state() const {return m_cache_receiptid;}    

 public:
    bool                    set_account_index(const std::string & account, const base::xaccount_index_t & account_index, base::xvcanvas_t* canvas);
    bool                    set_account_index(const std::string & account, const base::xaccount_index_t & account_index);
    bool                    set_receiptid_pair(base::xtable_shortid_t sid, const base::xreceiptid_pair_t & pair, base::xvcanvas_t* canvas);
    bool                    set_receiptid_pair(base::xtable_shortid_t sid, const base::xreceiptid_pair_t & pair);
    virtual int32_t        get_class_type() const override {return xstatistic::enum_statistic_table_bstate;}

private:
    size_t get_object_size_real() const override;

 protected:
    void                    cache_receiptid(base::xvbstate_t* bstate);

 private:
    // xobject_ptr_t<base::xvbstate_t> m_bstate;
    base::xreceiptid_state_ptr_t    m_cache_receiptid{nullptr};
};

using xtablestate_ptr_t = std::shared_ptr<xtable_bstate_t>;

NS_END2
