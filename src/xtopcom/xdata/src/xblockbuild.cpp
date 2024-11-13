// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include "xbase/xbase.h"
#include "xdata/xblockbuild.h"
#include "xdata/xrootblock.h"
#include "xdata/xemptyblock.h"
#include "xdata/xlightunit.h"
#include "xdata/xfullunit.h"
#include "xdata/xtableblock.h"
#include "xdata/xfull_tableblock.h"
#include "xdata/xnative_contract_address.h"
#include "xdata/xblockaction.h"
#include "xvledger/xventity.h"
#include "xvledger/xvaction.h"
#include "xvledger/xvcontract.h"
#include "xvledger/xvblock_fork.h"
#include "xvledger/xvblock_offdata.h"
#include "xdata/xblockextract.h"
#include "xpbase/base/tcash_utils.h"
#include "xcommon/xerror/xerror.h"
#include "xchain_fork/xutility.h"

NS_BEG2(tcash, data)

XINLINE_CONSTEXPR char const * BLD_URI_LIGHT_TABLE      = "b_lt//"; //xvcontract_t::create_contract_uri(b_lt, {}, 0)
XINLINE_CONSTEXPR char const * BLD_URI_FULL_TABLE       = "b_ft//"; //xvcontract_t::create_contract_uri(b_ft, {}, 0)
XINLINE_CONSTEXPR char const * BLD_URI_LIGHT_UNIT       = "b_lu//"; //xvcontract_t::create_contract_uri(b_lu, {}, 0)
XINLINE_CONSTEXPR char const * BLD_URI_FULL_UNIT        = "b_fu//"; //xvcontract_t::create_contract_uri(b_fu, {}, 0)
XINLINE_CONSTEXPR char const * BLD_URI_ROOT_BLOCK       = "b_rb//"; //xvcontract_t::create_contract_uri(b_rb, {}, 0)

std::string xtableheader_extra_build_t::build_extra_string(base::xvheader_t * _tableheader,
                                                     uint64_t tgas_height,
                                                     uint64_t gmtime,
                                                     const std::string & eth_header,
                                                     const std::string & output_offdata_hash,
                                                     const std::string & pledge_balance_change_tgas,
                                                     uint64_t burn_gas) {
    if (_tableheader->get_height() == 0) {
        // genesis block should not set extra
        return {};
    }

    base::xtableheader_extra_t header_extra;
    if (_tableheader->get_block_class() == base::enum_xvblock_class_light) {
        header_extra.set_tgas_total_lock_amount_property_height(tgas_height);
    }
    // after v3.0.0, only light-table set tgas height, and all class table should set gmtime
    header_extra.set_second_level_gmtime(gmtime);
    if (!base::xvblock_fork_t::is_block_older_version(_tableheader->get_block_version(), base::enum_xvblock_fork_version_compatible_eth) && !eth_header.empty()) {
        header_extra.set_ethheader(eth_header);
    }
    if(burn_gas > 0) {
        header_extra.set_total_burn_gas(burn_gas);
    }
    // after version_6_0_0, set output offdata hash to header extra data
    header_extra.set_output_offdata_hash(output_offdata_hash);
    // after version_6_0_0, set pledge_balance_change_tgas to header extra data
    header_extra.set_pledge_balance_change_tgas(pledge_balance_change_tgas);

    std::string extra_string;
    header_extra.serialize_to_string(extra_string);
    return extra_string;
}

int32_t xextra_map_base_t::serialize_to_string(std::string & str) const {
    base::xstream_t _stream(base::xcontext_t::instance());
    do_write(_stream);
    str.clear();
    str.assign((const char*)_stream.data(), _stream.size());
    return str.size();
}

int32_t xextra_map_base_t::do_write(base::xstream_t & stream) const {
    const int32_t begin_size = stream.size();
    stream << static_cast<uint32_t>(m_map.size());
    for (auto pair : m_map) {
        stream.write_compact_var(pair.first);
        stream.write_compact_var(pair.second);
    }
    return (stream.size() - begin_size);
}

int32_t xextra_map_base_t::serialize_from_string(const std::string & _data) {
    base::xstream_t _stream(base::xcontext_t::instance(),(uint8_t*)_data.data(),(uint32_t)_data.size());
    const int result = do_read(_stream);
    return result;
}

int32_t xextra_map_base_t::do_read(base::xstream_t & stream) {
    const int32_t begin_size = stream.size();
    uint32_t size;
    stream >> size;
    for (uint32_t i = 0; i < size; ++i) {
        std::string key;
        std::string val;
        stream.read_compact_var(key);
        stream.read_compact_var(val);
        m_map[key] = val;
    }
    return (begin_size - stream.size());
}

void xextra_map_base_t::insert(const std::string & key, const std::string & val) {
    m_map[key] = val;
}

std::string xextra_map_base_t::get_val(const std::string & key) const {
    auto it = m_map.find(key);
    if (it != m_map.end()) {
        return it->second;
    } else {
        return "";
    }
}

base::xvactions_t  xtable_primary_inentity_extend_t::get_txactions() const {
    auto value = get_val(KEY_ETH_RECEIPT_ACTIONS);
    if (value.empty()) {
        return {};
    }

    base::xvactions_t txactions;
    txactions.serialize_from_string(value);
    return txactions;
}
void    xtable_primary_inentity_extend_t::set_txactions(base::xvactions_t const& txactions) {
    if (txactions.get_actions().empty()){
        return;
    }
    std::string str;
    txactions.serialize_to_string(str);

    insert(KEY_ETH_RECEIPT_ACTIONS, str);
    xdbg("xtable_primary_inentity_extend_t::set_txactions txactions=%zu,serialize_size=%zu",txactions.get_actions().size(),str.size());
}

base::xvaction_t xblockaction_build_t::make_tx_action(const xcons_transaction_ptr_t & tx) {
    std::string caller_addr;  // empty means version0, no caller addr
    std::string contract_addr = tx->get_account_addr();
    std::string contract_name; // empty means version0, default contract
    uint32_t    contract_version = 0;
    std::string target_uri = base::xvcontract_t::create_contract_uri(contract_addr, contract_name, contract_version);
    std::string method_name = xtransaction_t::transaction_type_to_string(tx->get_tx_type());
    if (tx->is_recv_tx()) {  // set origin tx source addr for recv tx, confirm tx need know source addr without origin tx
        caller_addr = tx->get_source_addr();
    }

    base::xvalue_t _action_result(tx->get_tx_execute_state().get_map_para());  // how to set result
    base::xvaction_t _tx_action(tx->get_tx_hash(), caller_addr, target_uri, method_name);
    _tx_action.set_org_tx_action_id(tx->get_tx_subtype());
    _tx_action.copy_result(_action_result);
    return _tx_action;
}

base::xvaction_t xblockaction_build_t::make_block_build_action(const std::string & target_uri, const std::map<std::string, std::string> & action_result) {
    std::string caller_addr;  // empty means version0, no caller addr
    // std::string contract_addr;
    // std::string contract_name; // empty means version0, default contract
    // uint32_t    contract_version = 0;
    // std::string target_uri = xvcontract_t::create_contract_uri(contract_addr, contract_name, contract_version);
    std::string method_name = "b";
    std::string tx_hash;

    base::xvaction_t _tx_action(tx_hash, caller_addr, target_uri, method_name);
    if (!action_result.empty()) {
        base::xvalue_t _action_result(action_result);
        _tx_action.copy_result(_action_result);
    }
    xassert(!_tx_action.get_method_uri().empty());
    return _tx_action;
}

base::xvaction_t xblockaction_build_t::make_table_block_action_with_table_prop_prove(const std::string & target_uri, uint32_t block_version, const std::map<std::string, std::string> & property_hashs, base::xtable_shortid_t tableid, uint64_t height) {
    if (base::xvblock_fork_t::is_block_match_version(block_version, base::enum_xvblock_fork_version_table_prop_prove)) {
        // xassert(!para.get_property_hashs().empty());  // XTODO maybe empty for only self txs
        xassert(height > 0);
        xtableblock_action_t _action(target_uri, property_hashs, tableid, height);       
        return static_cast<base::xvaction_t>(_action);
    } else {
        base::xvaction_t _action = xblockaction_build_t::make_block_build_action(target_uri);
        return _action;
    }
}

xlightunit_tx_info_ptr_t xblockaction_build_t::build_tx_info(const xcons_transaction_ptr_t & tx) {
    base::xvaction_t _action = xblockaction_build_t::make_tx_action(tx);
    xlightunit_tx_info_ptr_t txinfo = std::make_shared<xlightunit_tx_info_t>(_action, tx->get_transaction());
    return txinfo;
}

xlightunit_build_t::xlightunit_build_t(const std::string & account, const xlightunit_block_para_t & bodypara) {
    base::xbbuild_para_t build_para(xrootblock_t::get_rootblock_chainid(), account, base::enum_xvblock_level_unit, base::enum_xvblock_class_light, xrootblock_t::get_rootblock_hash());
    init_header_qcert(build_para);
    build_block_body(bodypara);
}

xlightunit_build_t::xlightunit_build_t(base::xvblock_t* prev_block, const xlightunit_block_para_t & bodypara, const xblock_consensus_para_t & para) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_light, base::enum_xvblock_type_txs);
    build_para.set_unit_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                  para.get_drand_height(), para.get_parent_height(), para.get_justify_cert_hash());
    init_header_qcert(build_para);
    build_block_body(bodypara);
}
xlightunit_build_t::xlightunit_build_t(base::xvblock_t* prev_block, const xunit_block_para_t & bodypara, const xblock_consensus_para_t & para) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_light, base::enum_xvblock_type_txs);
    build_para.set_unit_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                  para.get_drand_height(), para.get_parent_height(), para.get_justify_cert_hash());
    init_header_qcert(build_para);
    build_block_body_v2(bodypara);
}
xlightunit_build_t::xlightunit_build_t(base::xvheader_t* header, base::xvinput_t* input, base::xvoutput_t* output)
: base::xvblockmaker_t(header, input, output) {

}

bool xlightunit_build_t::build_block_body(const xlightunit_block_para_t & para) {
    // #1 set input entitys and resources
    std::vector<base::xvaction_t> input_actions;
    if (base::xvblock_fork_t::is_block_older_version(get_header()->get_block_version(), base::enum_xvblock_fork_version_unit_opt)) {
        xdbg("block version:%d, height:%llu, account:%s", get_header()->get_block_version(), get_header()->get_height(), get_header()->get_account().c_str());
        for (auto & tx : para.get_input_txs()) {
            base::xvaction_t _action = xblockaction_build_t::make_tx_action(tx);
            input_actions.push_back(_action);
        }
        set_input_entity(input_actions);

        for (auto & tx : para.get_input_txs()) {
            // confirm tx no need take origintx
            if (tx->is_self_tx() || tx->is_send_tx()) {
                std::string origintx_bin;
                tx->get_transaction()->serialize_to_string(origintx_bin);
                std::string origintx_hash = tx->get_tx_hash();
                set_input_resource(origintx_hash, origintx_bin);
            }
        }

        // key_name_unconfirm_tx_count will be unusable after block 2.0.0
        uint32_t unconfirm_tx_num = para.get_account_unconfirm_sendtx_num();
        std::string unconfirm_tx_num_str = base::xstring_utl::tostring(unconfirm_tx_num);
        set_output_entity(base::xvoutentity_t::key_name_unconfirm_tx_count(), unconfirm_tx_num_str);
    } else {
        xdbg("block version:%d, height:%llu, account:%s", get_header()->get_block_version(), get_header()->get_height(), get_header()->get_account().c_str());
        base::xvaction_t _action = xblockaction_build_t::make_block_build_action(BLD_URI_LIGHT_UNIT);
        input_actions.push_back(_action);
        set_input_entity(input_actions);
    }

    // #2 set output entitys and resources
    set_output_full_state(para.get_fullstate_bin());
    set_output_binlog(para.get_property_binlog());
    return true;
}

bool xlightunit_build_t::build_block_body_v2(const xunit_block_para_t & para) {
    // #1 set input entitys and resources
    std::vector<base::xvaction_t> input_actions;
    xdbg("block version:%d, height:%llu, account:%s", get_header()->get_block_version(), get_header()->get_height(), get_header()->get_account().c_str());
    base::xvaction_t _action = xblockaction_build_t::make_block_build_action(BLD_URI_LIGHT_UNIT);
    input_actions.push_back(_action);
    set_input_entity(input_actions);
    // #2 set output entitys and resources
    if (!para.get_fullstate_bin_hash().empty()) {
        set_output_full_state_hash(para.get_fullstate_bin_hash());
    }
    if (!para.get_fullstate_bin().empty()) {
        set_output_full_state(para.get_fullstate_bin());
    }    
    set_output_binlog(para.get_property_binlog());
    return true;
}

base::xauto_ptr<base::xvblock_t> xlightunit_build_t::create_new_block() {
    return new xlightunit_block_t(*get_header(), *get_qcert(), get_input(), get_output());
}


xemptyblock_build_t::xemptyblock_build_t(const std::string & account) {
    base::enum_xvblock_level _level = get_block_level_from_account(account);
    base::xbbuild_para_t build_para(xrootblock_t::get_rootblock_chainid(), account, _level, base::enum_xvblock_class_nil, xrootblock_t::get_rootblock_hash());
    init_header_qcert(build_para);
}
xemptyblock_build_t::xemptyblock_build_t(const std::string & tc_account, uint64_t _tc_height) {
    xassert(_tc_height != 0);
    base::enum_xvblock_level _level = get_block_level_from_account(tc_account);
    base::enum_xvblock_type _type = get_block_type_from_empty_block(tc_account);
    base::xbbuild_para_t build_para(xrootblock_t::get_rootblock_chainid(), tc_account, _tc_height, base::enum_xvblock_class_nil, _level, _type, xrootblock_t::get_rootblock_hash(), xrootblock_t::get_rootblock_hash());
    xvip2_t _validator;
    set_empty_xip2(_validator);  // must change for block later
    build_para.set_basic_cert_para(_tc_height, -1, _tc_height, _validator);
    init_header_qcert(build_para);
}

xemptyblock_build_t::xemptyblock_build_t(const std::string & account, uint64_t height, uint64_t viewid, std::string const& extradata) {
    base::enum_xvblock_level _level = get_block_level_from_account(account);
    base::enum_xvblock_type _type = get_block_type_from_empty_block(account);
    base::xbbuild_para_t build_para(xrootblock_t::get_rootblock_chainid(), account, height, base::enum_xvblock_class_nil, _level, _type, xrootblock_t::get_rootblock_hash(), xrootblock_t::get_rootblock_hash());
    xvip2_t _validator;
    set_empty_xip2(_validator);  // must change for block later
    build_para.set_basic_cert_para(height, -1, viewid, _validator);  // relayblock has different content for different viewid, so should also set viewid
    init_header_qcert(build_para);
    set_header_extra(extradata);
}

xemptyblock_build_t::xemptyblock_build_t(base::xvblock_t* prev_block, const xblock_consensus_para_t & para) {
    base::enum_xvblock_type _type = get_block_type_from_empty_block(prev_block->get_account());
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_nil, _type);
    if (prev_block->get_block_level() == base::enum_xvblock_level_unit) {
        build_para.set_unit_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                    para.get_drand_height(), para.get_parent_height(), para.get_justify_cert_hash());
    } else if (prev_block->get_block_level() == base::enum_xvblock_level_table) {
        build_para.set_table_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                    para.get_drand_height(), para.get_justify_cert_hash());
    } else {
        build_para.set_basic_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator());
    }
    init_header_qcert(build_para);
    if ((prev_block->get_block_level() == base::enum_xvblock_level_table)) {
        std::string _extra_data = xtableheader_extra_build_t::build_extra_string(get_header(), para.get_tgas_height(), para.get_gmtime(), para.get_ethheader(), {}, {}, 0);
        set_header_extra(_extra_data);
    }
}

xemptyblock_build_t::xemptyblock_build_t(base::xvblock_t* prev_block) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_nil, base::enum_xvblock_type_general);
    // set for test
    build_para.set_basic_cert_para(prev_block->get_clock()+1, prev_block->get_viewtoken()+1, prev_block->get_viewid(), xvip2_t{(uint64_t)1, (uint64_t)1});
    init_header_qcert(build_para);
}
xemptyblock_build_t::xemptyblock_build_t(base::xvheader_t* header)
: base::xvblockmaker_t(header) {
}

base::xauto_ptr<base::xvblock_t> xemptyblock_build_t::create_new_block() {
    return new xemptyblock_t(*get_header(), *get_qcert());
}

xfullunit_build_t::xfullunit_build_t(base::xvblock_t* prev_block, const xfullunit_block_para_t & bodypara, const xblock_consensus_para_t & para) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_full, base::enum_xvblock_type_general);
    build_para.set_unit_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                    para.get_drand_height(), para.get_parent_height(), para.get_justify_cert_hash());
    
    init_header_qcert(build_para);
    build_block_body(bodypara);
}
xfullunit_build_t::xfullunit_build_t(base::xvblock_t* prev_block, const xunit_block_para_t & bodypara, const xblock_consensus_para_t & para) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_full, base::enum_xvblock_type_general);
    build_para.set_unit_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                    para.get_drand_height(), para.get_parent_height(), para.get_justify_cert_hash());
    
    init_header_qcert(build_para);
    build_block_body(bodypara);
}

xfullunit_build_t::xfullunit_build_t(base::xvheader_t* header, base::xvinput_t* input, base::xvoutput_t* output)
: base::xvblockmaker_t(header, input, output) {

}

bool xfullunit_build_t::build_block_body(const xunit_block_para_t & para) {
    // #1 set input entitys and resources
    base::xvaction_t _action = xblockaction_build_t::make_block_build_action(BLD_URI_FULL_UNIT);
    set_input_entity(_action);

    xdbg("fullunit block version:%d, height:%llu, account:%s", get_header()->get_block_version(), get_header()->get_height(), get_header()->get_account().c_str());
    // #2 set output entitys and resources
    std::string full_state_bin = para.get_fullstate_bin();
    set_output_full_state(full_state_bin);

    return true;
}

base::xauto_ptr<base::xvblock_t> xfullunit_build_t::create_new_block() {
    return new xfullunit_block_t(*get_header(), *get_qcert(), get_input(), get_output());
}

xunit_build2_t::xunit_build2_t(std::string const& account, uint64_t height, std::string const& last_block_hash, bool is_full_unit, const xblock_consensus_para_t & para) {
    xassert(para.get_parent_height() > 0);
    if (false == chain_fork::xutility_t::is_forked(fork_points::v11200_block_fork_point, para.get_clock())) {
        base::enum_xvblock_class _class = is_full_unit ? base::enum_xvblock_class_full : base::enum_xvblock_class_light;
        base::enum_xvblock_level _level = base::enum_xvblock_level_unit;
        base::enum_xvblock_type  _type = base::enum_xvblock_type_txs;
        base::xbbuild_para_t build_para(xrootblock_t::get_rootblock_chainid(), account, height, _class, _level, _type, last_block_hash, std::string());
        build_para.set_unit_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                        para.get_drand_height(), para.get_parent_height(), std::string());
        
        init_header_qcert(build_para);
    } else {
        // unit is nil block but with binlog
        base::enum_xvblock_class _class = base::enum_xvblock_class_nil;
        base::enum_xvblock_level _level = base::enum_xvblock_level_unit;
        base::enum_xvblock_type  _type = is_full_unit ? base::enum_xvblock_type_fullunit : base::enum_xvblock_type_lightunit;
        base::xbbuild_para_t build_para(xrootblock_t::get_rootblock_chainid(), account, height, _class, _level, _type, last_block_hash, std::string());
        // qcert only has clock,viewid,parent height
        build_para.set_simple_cert_para(para.get_clock(), para.get_viewid(), para.get_parent_height());        
        init_header_qcert(build_para);  
        set_block_character(base::enum_xvblock_character_simple_unit);
    }    
}

xunit_build2_t::xunit_build2_t(base::xvheader_t* header, base::xvblock_t* parentblock, const xunit_block_para_t & bodypara) {
    if (false == header->is_character_simple_unit()) {
        base::xbbuild_para_t build_para;
        // TODO(jimmy) should has no cert members
        build_para.set_unit_cert_para(parentblock->get_clock(), parentblock->get_viewtoken(), parentblock->get_viewid(), parentblock->get_cert()->get_validator(), parentblock->get_cert()->get_auditor(),
                                    parentblock->get_cert()->get_drand_height(), parentblock->get_height(), std::string());
        set_header(header);
        init_qcert(build_para);
        build_block_body(bodypara);
    } else {
        base::xbbuild_para_t build_para;
        build_para.set_simple_cert_para(parentblock->get_clock(), parentblock->get_viewid(), parentblock->get_height());
        set_header(header);
        init_qcert(build_para);
    }
}

void xunit_build2_t::create_block_body(const xunit_block_para_t & bodypara) {
    if (false == chain_fork::xutility_t::is_forked(fork_points::v11200_block_fork_point, get_qcert()->get_clock())) {
        build_block_body(bodypara);
    } else {
        build_block_body_for_simple_unit(bodypara);  
    }
}

bool xunit_build2_t::build_block_body(const xunit_block_para_t & para) {
    // TODO(jimmy) should has no entitys and resources
    
    // #1 set input entitys and resources
    std::vector<base::xvaction_t> input_actions;
    xdbg("block version:%d, height:%llu, account:%s", get_header()->get_block_version(), get_header()->get_height(), get_header()->get_account().c_str());
    base::xvaction_t _action = xblockaction_build_t::make_block_build_action(BLD_URI_LIGHT_UNIT);
    input_actions.push_back(_action);
    set_input_entity(input_actions);   
    // #2 set output entitys and resources

    if (get_header()->get_block_class() == base::enum_xvblock_class_light) {
        set_output_binlog(para.get_property_binlog());
    }
    if (!para.get_fullstate_bin().empty()) {
        set_output_full_state(para.get_fullstate_bin());
    } else {
        xassert(!para.get_fullstate_bin_hash().empty());
        set_output_full_state_hash(para.get_fullstate_bin_hash());
    }    
    return true;
}

bool xunit_build2_t::build_block_body_for_simple_unit(const xunit_block_para_t & para) {
    base::xunit_header_extra_t _header_extra;
    // always set binlog only for fullunit offchain state
    _header_extra.set_binlog(para.get_property_binlog());
    std::string _state_hash = para.get_fullstate_bin_hash();
    assert(!_state_hash.empty());
    _header_extra.set_state_hash(_state_hash);
    std::string header_extra_bin;
    _header_extra.serialize_to_string(header_extra_bin);
    set_header_extra(header_extra_bin);

    // TODO(jimmy) state hash set to qcert output root hash
    return true;
}

base::xauto_ptr<base::xvblock_t> xunit_build2_t::create_new_block() {
    if (get_header()->get_block_class() == base::enum_xvblock_class_nil) {
        return new xemptyblock_t(*get_header(), *get_qcert());
    } else if (get_header()->get_block_class() == base::enum_xvblock_class_light) {
        return new xlightunit_block_t(*get_header(), *get_qcert(), get_input(), get_output());
    } else if (get_header()->get_block_class() == base::enum_xvblock_class_full) {
        return new xfullunit_block_t(*get_header(), *get_qcert(), get_input(), get_output());
    }
    xassert(false);
    return nullptr;
}

base::xauto_ptr<base::xvinput_t> xlighttable_build_t::make_unit_input_from_table(const base::xvblock_t* _tableblock, const base::xtable_inentity_extend_t & extend, base::xvinentity_t* _table_unit_inentity) {
    const std::vector<base::xvaction_t> &  input_actions = _table_unit_inentity->get_actions();
    // make unit input entity by table input entity
    base::xauto_ptr<base::xvinentity_t> _unit_inentity = new base::xvinentity_t(input_actions);
    std::vector<base::xventity_t*> _unit_inentitys;
    _unit_inentitys.push_back(_unit_inentity.get());

    // query unit input resource from table and make unit resource
    base::xauto_ptr<base::xstrmap_t> _strmap = new base::xstrmap_t();
    for (auto & action : input_actions) {
        if (!action.get_org_tx_hash().empty()) {
            std::string _org_tx = _tableblock->query_input_resource(action.get_org_tx_hash());
            if (_org_tx.empty()) {
                // confirm tx not has origin tx
                base::enum_transaction_subtype _subtype = (base::enum_transaction_subtype)action.get_org_tx_action_id();
                if (_subtype == base::enum_transaction_subtype_send) { // sendtx must has origin tx
                    xassert(false);
                    return nullptr;
                }
            }
            _strmap->set(action.get_org_tx_hash(), _org_tx);
        }
    }

    base::xauto_ptr<base::xvinput_t> _unit_input = make_object_ptr<base::xvinput_t>(_unit_inentitys, _strmap.get());
    return _unit_input;
}

base::xauto_ptr<base::xvoutput_t> xlighttable_build_t::make_unit_output_from_table(const base::xvblock_t* _tableblock, const base::xtable_inentity_extend_t & extend, base::xvoutentity_t* _table_unit_outentity) {
    // make unit output entity by table output entity
    base::xauto_ptr<base::xvoutentity_t> _unit_outentity = new base::xvoutentity_t(*_table_unit_outentity);
    std::vector<base::xventity_t*> _unit_outentitys;
    _unit_outentitys.push_back(_unit_outentity.get());

    // query unit input resource from table and make unit resource
    base::xauto_ptr<base::xstrmap_t> _strmap = new base::xstrmap_t();
    if (extend.get_unit_header()->get_block_class() == base::enum_xvblock_class_light) {
        std::string _binlog = _tableblock->query_output_resource(_unit_outentity->get_binlog_hash());
        if (!_binlog.empty()) {
            _strmap->set(_unit_outentity->get_binlog_hash(), _binlog);
        } else {
            xassert(false);
            return nullptr;  // lightunit must has binlog
        }
    }
    if (extend.get_unit_header()->get_block_class() == base::enum_xvblock_class_full) {
        std::string _state = _tableblock->query_output_resource(extend.get_unit_output_root_hash());
        if (!_state.empty()) {
            _strmap->set(extend.get_unit_output_root_hash(), _state);
        } else {
            xassert(false);
            return nullptr;  // fullunit must has fullstate
        }
    }
    xobject_ptr_t<base::xvoutput_t> _unit_output = make_object_ptr<base::xvoutput_t>(_unit_outentitys, _strmap.get());
    _unit_output->set_root_hash(extend.get_unit_output_root_hash());
    return _unit_output;
}

std::vector<xobject_ptr_t<base::xvblock_t>> xlighttable_build_t::unpack_units_from_table(const base::xvblock_t* _tableblock) {
    XMETRICS_GAUGE(metrics::data_table_unpack_units, 1);

    if (!_tableblock->is_body_and_offdata_ready(false)) {
        xerror("xlighttable_build_t::unpack_units_from_table not ready block. block=%s", _tableblock->dump().c_str());
        return {};
    }

    base::xvaccount_t _vtable_addr(_tableblock->get_account());
    std::vector<xobject_ptr_t<base::xvblock_t>> _batch_units;

    std::error_code ec;
    auto input_object = _tableblock->load_input(ec);
    auto output_object = _tableblock->load_output(ec);
    if (nullptr == input_object || nullptr == output_object) {
        xerror("xlighttable_build_t::unpack_units_from_table input or output nullptr. block=%s", _tableblock->dump().c_str());
        return {};        
    }

    const std::vector<base::xventity_t*> & _table_inentitys = input_object->get_entitys();
    const std::vector<base::xventity_t*> & _table_outentitys = output_object->get_entitys();
    if (_table_inentitys.size() <= 0 || _table_inentitys.size() != _table_outentitys.size()) {
        xassert(false);
        return {};
    }
    uint32_t entitys_count = _table_inentitys.size();
    for (uint32_t index = 1; index < entitys_count; index++) {  // unit entity from index#1
        base::xvinentity_t* _table_unit_inentity = dynamic_cast<base::xvinentity_t*>(_table_inentitys[index]);
        base::xvoutentity_t* _table_unit_outentity = dynamic_cast<base::xvoutentity_t*>(_table_outentitys[index]);
        if (_table_unit_inentity == nullptr || _table_unit_outentity == nullptr) {
            xassert(false);
            return {};
        }

        base::xtable_inentity_extend_t extend;
        extend.serialize_from_string(_table_unit_inentity->get_extend_data());
        const xobject_ptr_t<base::xvheader_t> & _unit_header = extend.get_unit_header();

        std::shared_ptr<base::xvblockmaker_t> vbmaker = nullptr;
        if (_unit_header->get_block_class() == base::enum_xvblock_class_nil) {
            vbmaker = std::make_shared<xemptyblock_build_t>(_unit_header.get());
        } else {
            base::xauto_ptr<base::xvinput_t> _unit_input = make_unit_input_from_table(_tableblock, extend, _table_unit_inentity);
            base::xauto_ptr<base::xvoutput_t> _unit_output = make_unit_output_from_table(_tableblock, extend, _table_unit_outentity);
            if (_unit_input == nullptr || _unit_output == nullptr) {
                xassert(false);
                return {};
            }
            if (_unit_header->get_block_class() == base::enum_xvblock_class_light) {
                vbmaker = std::make_shared<xlightunit_build_t>(_unit_header.get(), _unit_input.get(), _unit_output.get());
            } else if (_unit_header->get_block_class() == base::enum_xvblock_class_full) {
                vbmaker = std::make_shared<xfullunit_build_t>(_unit_header.get(), _unit_input.get(), _unit_output.get());
            }
        }

        base::xbbuild_para_t build_para;
        base::xvqcert_t* tablecert = _tableblock->get_cert();
        build_para.set_unit_cert_para(tablecert->get_clock(), tablecert->get_viewtoken(), tablecert->get_viewid(), tablecert->get_validator(),
                                    tablecert->get_auditor(), tablecert->get_drand_height(), _tableblock->get_height(), extend.get_unit_justify_hash());
        vbmaker->init_qcert(build_para);
        xobject_ptr_t<base::xvblock_t> _unit = vbmaker->build_new_block();
        xassert(_unit != nullptr);
        _unit->set_parent_block(_vtable_addr.get_account(), index);
        _unit->get_cert()->set_parent_height(_tableblock->get_height());
        _unit->get_cert()->set_parent_viewid(_tableblock->get_viewid());

        _batch_units.push_back(_unit);
    }

    base::xvtableblock_maker_t::units_set_parent_cert(_batch_units, _tableblock);
#ifdef DEBUG
    for (auto & v : _batch_units) {
        xassert(!v->get_cert()->get_extend_cert().empty());
        xassert(!v->get_cert()->get_extend_data().empty());
    }
#endif
    return _batch_units;
}

std::vector<xobject_ptr_t<base::xvblock_t>> xtable_build2_t::unpack_units_from_table(base::xvblock_t* _tableblock, std::error_code & ec) {
    if (_tableblock->get_output_offdata_hash().empty()) {
        return {};
    }

    if (_tableblock->get_output_offdata().empty()) {
        ec = common::error::xerrc_t::invalid_block;
        xerror("xtable_build2_t::unpack_units_from_table fail-has no output offdata.%s",_tableblock->dump().c_str());
        return {};
    }

    base::xvblock_out_offdata_t offdata;
    offdata.serialize_from_string(_tableblock->get_output_offdata());

    auto subblocks_info = offdata.get_subblocks_info();
    std::vector<xobject_ptr_t<base::xvblock_t>> subblocks;
    for (auto & subblock_info : subblocks_info) {
        if (nullptr == subblock_info.get_header()) {
            ec = common::error::xerrc_t::invalid_block;
            xerror("xtable_build2_t::unpack_units_from_table fail-has unit header.%s",_tableblock->dump().c_str());
            return {};            
        }
        xunit_block_para_t body_para;
        if (subblock_info.get_header()->get_block_class() == base::enum_xvblock_class_full) { 
            body_para.set_fullstate_bin(subblock_info.get_binlog());
        } else {
            body_para.set_binlog(subblock_info.get_binlog());
        }
        body_para.set_fullstate_bin_hash(subblock_info.get_state_hash());
        
        std::shared_ptr<base::xvblockmaker_t> vblockmaker = std::make_shared<data::xunit_build2_t>(subblock_info.get_header().get(), _tableblock, body_para);
        xobject_ptr_t<base::xvblock_t> _new_block = vblockmaker->build_new_block();        
        if (_new_block->get_cert()->is_consensus_flag_has_extend_cert()) {
            _new_block->get_cert()->set_parent_height(_tableblock->get_height());
            _new_block->get_cert()->set_parent_viewid(_tableblock->get_viewid());
            _new_block->set_extend_cert("1");
            _new_block->set_extend_data("1");
        }
        _new_block->set_block_flag(base::enum_xvblock_flag_authenticated);
        subblocks.push_back(_new_block);
    }

#ifdef DEBUG // XTODO check if unit match accountindex in debug mode
    base::xaccount_indexs_t account_indexs;
    auto account_indexs_str = _tableblock->get_account_indexs();
    if (!account_indexs_str.empty()) {
        account_indexs.serialize_from_string(account_indexs_str);
    }
    for (uint32_t i = 0; i < subblocks.size(); i++) {
        if (!account_indexs.get_account_indexs().empty()) {
            auto & accountindex = account_indexs.get_account_indexs()[i].second;
            if (accountindex.get_latest_unit_hash() != subblocks[i]->build_block_hash()
                || accountindex.get_latest_unit_height() != subblocks[i]->get_height()
                || accountindex.get_latest_unit_hash() != subblocks[i]->get_block_hash()
                || account_indexs.get_account_indexs()[i].first != subblocks[i]->get_account()) {
                xerror("xtable_build2_t::unpack_units_from_table fail-unmatch.%s,%s",accountindex.dump().c_str(),subblocks[i]->dump().c_str());
            }
        }
    }

#endif

    xassert(!subblocks.empty());
    return subblocks;
}

xtable_build2_t::xtable_build2_t(base::xvblock_t* prev_block, const xtable_block_para_t & bodypara, const xblock_consensus_para_t & para) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_light, base::enum_xvblock_type_batch);

    build_para.set_table_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                    para.get_drand_height(), para.get_justify_cert_hash(), para.need_relay_prove());
    base::xvaccount_t _vaccount(prev_block->get_account());
    init_header_qcert(build_para);
    build_block_body(bodypara, para);
}

base::xauto_ptr<base::xvblock_t> xtable_build2_t::create_new_block() {
    return new xtable_block_t(*get_header(), *get_qcert(), get_input(), get_output());
}

bool xtable_build2_t::build_block_body(const xtable_block_para_t & para, const xblock_consensus_para_t & cs_para) {
    assert(!cs_para.get_table_account().empty());
    base::xvaccount_t account(cs_para.get_table_account());
    std::string out_offdata_bin;
    std::string account_indexs_str;
    std::string output_offdata_hash;
    if (!para.get_units().empty()) {
        base::xvblock_out_offdata_t offdata(para.get_units());
        offdata.serialize_to_string(out_offdata_bin);
        output_offdata_hash = get_qcert()->hash(out_offdata_bin);
    }
    if (!para.get_accountindexs().get_account_indexs().empty()) {
        para.get_accountindexs().serialize_to_string(account_indexs_str);
    }
    
    std::string header_extra_data;
    if (base::xvblock_fork_t::is_block_older_version(get_header()->get_block_version(), base::enum_xvblock_fork_version_6_0_0)) {
        header_extra_data =  xtableheader_extra_build_t::build_extra_string(
        get_header(), cs_para.get_tgas_height(), cs_para.get_gmtime(), cs_para.get_ethheader(), {}, {}, cs_para.get_total_burn_gas());
        set_output_entity(base::xvoutentity_t::key_name_output_offdata_hash(), output_offdata_hash);
        std::string tgas_balance_change = base::xstring_utl::tostring(para.get_tgas_balance_change());
        set_output_entity(base::xvoutentity_t::key_name_tgas_pledge_change(), tgas_balance_change);        
    } else {
        std::string tgas_balance_change = base::xstring_utl::tostring(para.get_tgas_balance_change());       
        header_extra_data =  xtableheader_extra_build_t::build_extra_string(
        get_header(), cs_para.get_tgas_height(), cs_para.get_gmtime(), cs_para.get_ethheader(), output_offdata_hash, tgas_balance_change, cs_para.get_total_burn_gas());
    }
    set_header_extra(header_extra_data);

    // #1 set input entitys and resources
    base::xvaction_t _action = xblockaction_build_t::make_table_block_action_with_table_prop_prove(BLD_URI_LIGHT_TABLE, get_header()->get_block_version(), para.get_property_hashs(), account.get_short_table_id(), get_header()->get_height());
    xdbg("xtable_build2_t::build_block_body. account=%s,height=%ld,version=%ld,tx size:%zu", account.get_account().c_str(), get_header()->get_height(), get_header()->get_block_version(), para.get_txs()->size());
    std::vector<base::xvaction_t> input_actions;
    input_actions.push_back(_action);

    xtable_primary_inentity_extend_t _tpinentity_ext;
    std::string primary_inentity_extend_bin;

    // rule:eth_receipt_txactions put to primary inentity extend
    base::xvactions_t eth_receipt_txactions;
    for (auto & tx : *para.get_txs()) {
        // TODO(jimmy) only v3 tx put to 
        if ( (tx->get_raw_tx() != nullptr)
            && (tx->get_raw_tx()->get_tx_version() == xtransaction_version_3)) {
            eth_receipt_txactions.add_action(*tx.get());
        } else {
            input_actions.push_back(*tx.get());
        }
    }
    _tpinentity_ext.set_txactions(eth_receipt_txactions);
    xdbg("xtable_build2_t::build_block_body eth_count=%zu,other_count=%zu",eth_receipt_txactions.get_actions().size(),input_actions.size());

    if (!_tpinentity_ext.get_all_val().empty()) {
        _tpinentity_ext.serialize_to_string(primary_inentity_extend_bin);
    }
    set_input_entity(input_actions, primary_inentity_extend_bin);
    // TODO(jimmy) txactions move to resource

    for (auto & tx : *para.get_txs()) {
        // confirm tx no need take origintx
        if (tx->is_self_tx() || tx->is_send_tx()) {
            std::string origintx_bin;
            tx->get_raw_tx()->serialize_to_string(origintx_bin);
            std::string origintx_hash = tx->get_tx_hash();
            set_input_resource(origintx_hash, origintx_bin);
        }
    }

    // #2 set output entitys and resources
    std::string binlog = para.get_property_binlog();
    if (!binlog.empty()) {
        std::string binlog_hash = base::xcontext_t::instance().hash(binlog, get_qcert()->get_crypto_hash_type());
        set_output_entity(base::xvoutentity_t::key_name_binlog_hash(), binlog_hash);
        set_output_resource(binlog_hash, binlog);        
        // set_output_resource(RESOURCE_TABLE_BINLOG, binlog)  // TODO(jimmy) 
    }
    std::string full_state = para.get_fullstate_bin();
    if (!full_state.empty()) {
        set_output_full_state(full_state);
        // auto full_state_hash = base::xcontext_t::instance().hash(full_state, get_qcert()->get_crypto_hash_type());
        // set_output_entity(base::xvoutentity_t::key_name_state_hash(), full_state_hash);
    }

    if (!account_indexs_str.empty()) {
        set_output_resource(base::xvoutput_t::RESOURCE_ACCOUNT_INDEXS, account_indexs_str);
    }

    if (!out_offdata_bin.empty()) {
        set_output_offdata(out_offdata_bin);
    }   

    for (auto & v : para.get_input_resources()) {
        set_input_resource(v.first, v.second);
    }
    for (auto & v : para.get_output_resources()) {
        set_output_resource(v.first, v.second);
    }

    return true;
}

xfulltable_build_t::xfulltable_build_t(base::xvblock_t* prev_block, const xfulltable_block_para_t & bodypara, const xblock_consensus_para_t & para) {
    base::xbbuild_para_t build_para(prev_block, base::enum_xvblock_class_full, base::enum_xvblock_type_general);
    build_para.set_table_cert_para(para.get_clock(), para.get_viewtoken(), para.get_viewid(), para.get_validator(), para.get_auditor(),
                                    para.get_drand_height(), para.get_justify_cert_hash());
    base::xvaccount_t _vaccount(prev_block->get_account());
    init_header_qcert(build_para);
    build_block_body(bodypara, para);
}

bool xfulltable_build_t::build_block_body(const xfulltable_block_para_t & para, const xblock_consensus_para_t & cs_para) {
    base::xvaccount_t _vaccount(get_header()->get_account());
    std::string header_extra_data;
    if (base::xvblock_fork_t::is_block_older_version(get_header()->get_block_version(), base::enum_xvblock_fork_version_6_0_0)) {
        header_extra_data =  xtableheader_extra_build_t::build_extra_string(
        get_header(), cs_para.get_tgas_height(), cs_para.get_gmtime(), cs_para.get_ethheader(), {}, {}, cs_para.get_total_burn_gas());
        std::string tgas_balance_change = base::xstring_utl::tostring(para.get_tgas_balance_change());
        set_output_entity(base::xvoutentity_t::key_name_tgas_pledge_change(), tgas_balance_change);        
    } else {
        std::string tgas_balance_change = base::xstring_utl::tostring(para.get_tgas_balance_change());       
        header_extra_data =  xtableheader_extra_build_t::build_extra_string(
        get_header(), cs_para.get_tgas_height(), cs_para.get_gmtime(), cs_para.get_ethheader(), {}, tgas_balance_change, cs_para.get_total_burn_gas());
    }
    set_header_extra(header_extra_data);

    // #1 set input entitys and resources
    base::xvaction_t _action = xblockaction_build_t::make_table_block_action_with_table_prop_prove(BLD_URI_FULL_TABLE, get_header()->get_block_version(), para.get_property_hashs(), _vaccount.get_short_table_id(), get_header()->get_height());
    set_input_entity(_action);
    xdbg("xfulltable_build_t::build_block_body,account=%s,height=%ld,version=0x%x", get_header()->get_account().c_str(), get_header()->get_height(), get_header()->get_block_version());
    // #2 set output entitys and resources
    std::string full_state_bin = para.get_snapshot();
    set_output_full_state(full_state_bin);

    #ifndef  XBUILD_CONSORTIUM
        const xstatistics_data_t & statistics_data = para.get_block_statistics_data();
    #else
        const xstatistics_cons_data_t & statistics_data = para.get_block_statistics_cons_data();
    #endif  
    auto const & serialized_data = statistics_data.serialize_based_on<base::xstream_t>();
    std::string serialized_data_str = {std::begin(serialized_data), std::end(serialized_data) };
    set_input_resource(base::xvinput_t::RESOURCE_NODE_SIGN_STATISTICS, serialized_data_str);
    return true;
}

base::xauto_ptr<base::xvblock_t> xfulltable_build_t::create_new_block() {
    return new xfull_tableblock_t(*get_header(), *get_qcert(), get_input(), get_output());
}


xrootblock_build_t::xrootblock_build_t(base::enum_xchain_id chainid, const std::string & account, const xrootblock_para_t & bodypara) {
    // rootblock last block hash is empty
    base::xbbuild_para_t build_para(chainid, account, base::enum_xvblock_level_chain, base::enum_xvblock_class_light, std::string());
    init_header_qcert(build_para);
    build_block_body(bodypara);
}

bool xrootblock_build_t::build_block_body(const xrootblock_para_t & para) {
    // #1 set input entitys and resources
    base::xvaction_t _action = xblockaction_build_t::make_block_build_action(BLD_URI_ROOT_BLOCK);
    set_input_entity(_action);

    xobject_ptr_t<xrootblock_input_t> input = make_object_ptr<xrootblock_input_t>();
    input->set_account_balances(para.m_account_balances);
    input->set_genesis_funds_accounts(para.m_geneis_funds_accounts);
    input->set_genesis_tcc_accounts(para.m_tcc_accounts);
    input->set_genesis_nodes(para.m_genesis_nodes);
    if(para.m_extend_data_map.size() > 0) {
        input->set_extend_data_map(para.m_extend_data_map);
    }
    // TODO(jimmy) use bstate
    std::string resource_bin;
    input->serialize_to_string(resource_bin);
    // set_input_resource(xrootblock_t::root_resource_name, resource_bin);
    // #3 set output entitys and resources

    // std::string binlog = "1"; // TODO(jimmy) just for rules
    // set_output_binlog(binlog);
    // std::string full_state = "2"; // TODO(jimmy) just for rules not same hash with binlog
    // set_output_full_state(full_state);


    xobject_ptr_t<base::xvbstate_t> bstate = make_object_ptr<base::xvbstate_t>(*get_header());
    xobject_ptr_t<base::xvcanvas_t> canvas = make_object_ptr<base::xvcanvas_t>();
    {
        auto propobj = bstate->new_string_var(xrootblock_t::ROOT_BLOCK_PROPERTY_NAME, canvas.get());
        propobj->reset(resource_bin, canvas.get());
    }

    std::string property_binlog;
    canvas->encode(property_binlog);
    std::string fullstate_bin;
    bstate->take_snapshot(fullstate_bin);
    set_output_binlog(property_binlog);
    set_output_full_state(fullstate_bin);
    return true;
}

base::xauto_ptr<base::xvblock_t> xrootblock_build_t::create_new_block() {
    return new xrootblock_t(*get_header(), *get_qcert(), get_input(), get_output());
}

NS_END2
