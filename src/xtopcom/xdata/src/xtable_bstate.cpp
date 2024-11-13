// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include "xbasic/xmodule_type.h"
#include "xstatistic/xbasic_size.hpp"
#include "xdata/xtable_bstate.h"
#include "xmetrics/xmetrics.h"

NS_BEG2(tcash, data)

xtable_bstate_t::xtable_bstate_t(base::xvbstate_t* bstate)
: xbstate_ctx_t(bstate), xstatistic::xstatistic_obj_face_t(xstatistic::enum_statistic_table_bstate) {
    cache_receiptid(bstate); // TODO(jimmy) delete future
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_table_state, 1);
}

xtable_bstate_t::xtable_bstate_t(base::xvbstate_t* bstate, base::xvbstate_t* org_bstate)
: xbstate_ctx_t(bstate, org_bstate), xstatistic::xstatistic_obj_face_t(xstatistic::enum_statistic_table_bstate) {
    cache_receiptid(bstate); // TODO(jimmy) delete future
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_table_state, 1);
}

xtable_bstate_t::~xtable_bstate_t() {
    statistic_del();
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_table_state, -1);
}

bool xtable_bstate_t::set_account_index(const std::string & account, const base::xaccount_index_t & account_index, base::xvcanvas_t* canvas) {
    std::string value;
    account_index.old_serialize_to(value);

    if (false == get_bstate()->find_property(XPROPERTY_TABLE_ACCOUNT_INDEX)) {
        auto propobj = get_bstate()->new_string_map_var(XPROPERTY_TABLE_ACCOUNT_INDEX, canvas);
        xassert(propobj != nullptr);
    }
    auto propobj = get_bstate()->load_string_map_var(XPROPERTY_TABLE_ACCOUNT_INDEX);  // TODO(jimmy) use hash map
    auto old_value = propobj->query(account);
    xassert(old_value != value);
    propobj->insert(account, value, canvas);
    return true;
}

bool xtable_bstate_t::set_account_index(const std::string & account, const base::xaccount_index_t & account_index) {
    std::string value;
    account_index.old_serialize_to(value);

    if (false == get_bstate()->find_property(XPROPERTY_TABLE_ACCOUNT_INDEX)) {
        get_bstate()->new_string_map_var(XPROPERTY_TABLE_ACCOUNT_INDEX, get_canvas().get());
    }

    map_set(XPROPERTY_TABLE_ACCOUNT_INDEX, account, value);
    return true;
}

bool xtable_bstate_t::get_account_index(const std::string & account, base::xaccount_index_t & account_index) const {
    std::string value = map_get(XPROPERTY_TABLE_ACCOUNT_INDEX, account);
    if (value.empty()) {
        return false;
    }
    account_index.old_serialize_from(value);
    return true;
}

std::set<std::string> xtable_bstate_t::get_all_accounts() const {
    std::set<std::string> all_accounts;
    std::map<std::string,std::string> values = map_get(XPROPERTY_TABLE_ACCOUNT_INDEX);
    for (auto & v : values) {
        base::xaccount_index_t account_index;
        account_index.old_serialize_from(v.second);
        all_accounts.insert(v.first);
    }
    return all_accounts;
}

std::vector<std::pair<common::xaccount_address_t, base::xaccount_index_t>> xtable_bstate_t::all_accounts() const {
    std::vector<std::pair<common::xaccount_address_t, base::xaccount_index_t>> all_accounts;
    auto const & values = map_get(XPROPERTY_TABLE_ACCOUNT_INDEX);
    for (auto const & v : values) {
        base::xaccount_index_t account_index;
        account_index.serialize_from(v.second);
        all_accounts.emplace_back(common::xaccount_address_t::build_from(v.first), std::move(account_index));
    }

    return all_accounts;
}

int32_t xtable_bstate_t::get_account_size() const {
    int32_t size = 0;
    map_size(XPROPERTY_TABLE_ACCOUNT_INDEX, size);
    return size;
}

bool xtable_bstate_t::find_receiptid_pair(base::xtable_shortid_t sid, base::xreceiptid_pair_t & pair) const {
    std::string field = base::xstring_utl::tostring(sid);
    std::string value = map_get(XPROPERTY_TABLE_RECEIPTID, field);
    if (value.empty()) {
        return false;
    }
    pair.serialize_from(value);
    return true;
}

void xtable_bstate_t::cache_receiptid(base::xvbstate_t* bstate) {
    m_cache_receiptid = make_receiptid_from_state(bstate);
}

base::xreceiptid_state_ptr_t xtable_bstate_t::make_receiptid_from_state(base::xvbstate_t* bstate) {
    base::xreceiptid_state_ptr_t receiptid = std::make_shared<base::xreceiptid_state_t>(bstate->get_short_table_id(), bstate->get_block_height());
    if (false == bstate->find_property(XPROPERTY_TABLE_RECEIPTID)) {
        return receiptid;
    }
    auto propobj = bstate->load_string_map_var(XPROPERTY_TABLE_RECEIPTID);
    auto all_values = propobj->query();
    for (auto & v : all_values) {
        base::xtable_shortid_t sid = (base::xtable_shortid_t)base::xstring_utl::touint32(v.first);
        base::xreceiptid_pair_t pair;
        pair.serialize_from(v.second);
        receiptid->add_pair(sid, pair);
    }
    receiptid->update_unconfirm_tx_num();  // calc and cache unconfirm tx for get performance
    return receiptid;
}

std::string xtable_bstate_t::get_receiptid_property_bin(base::xvbstate_t* bstate) {
    std::string property_receiptid_bin;
    if (bstate->find_property(XPROPERTY_TABLE_RECEIPTID)) {
        auto propobj = bstate->load_property(XPROPERTY_TABLE_RECEIPTID);
        if (propobj != nullptr) {
            propobj->serialize_to_string(property_receiptid_bin);
        }
    }
    return property_receiptid_bin;
}

bool xtable_bstate_t::set_receiptid_pair(base::xtable_shortid_t sid, const base::xreceiptid_pair_t & pair, base::xvcanvas_t* canvas) {
    if (false == get_bstate()->find_property(XPROPERTY_TABLE_RECEIPTID)) {
        auto propobj = get_bstate()->new_string_map_var(XPROPERTY_TABLE_RECEIPTID, canvas);
        xassert(propobj != nullptr);
    }

    std::string field = base::xstring_utl::tostring(sid);
    std::string value;
    pair.serialize_to(value);

    auto propobj = get_bstate()->load_string_map_var(XPROPERTY_TABLE_RECEIPTID);
    auto old_value = propobj->query(field);
    xassert(old_value != value);
    propobj->insert(field, value, canvas);
    return true;
}

bool xtable_bstate_t::set_receiptid_pair(base::xtable_shortid_t sid, const base::xreceiptid_pair_t & pair) {
    std::string field = base::xstring_utl::tostring(sid);
    std::string value;
    pair.serialize_to(value);

    if (false == get_bstate()->find_property(XPROPERTY_TABLE_RECEIPTID)) {
        get_bstate()->new_string_map_var(XPROPERTY_TABLE_RECEIPTID, get_canvas().get());
    }

    int32_t ret = map_set(XPROPERTY_TABLE_RECEIPTID, field, value);
    return ret == xsuccess;
}

size_t xtable_bstate_t::get_object_size_real() const {
    size_t total_size = sizeof(*this);
    int32_t cache_receiptid_size = 0;
    if (m_cache_receiptid != nullptr) {
        cache_receiptid_size = m_cache_receiptid->get_object_size_real();
    }

    total_size += cache_receiptid_size;

    xdbg("------cache size------ xtable_bstate_t total_size:%zu this:%d,cache_receiptid_size:%d", total_size, sizeof(*this), cache_receiptid_size);
    return total_size;
}

NS_END2
