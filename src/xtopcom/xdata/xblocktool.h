// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <vector>

#include "xchain_upgrade/xchain_data_processor.h"
#include "xdata/xcons_transaction.h"
#include "xdata/xemptyblock.h"
#include "xdata/xfull_tableblock.h"
#include "xdata/xfullunit.h"
#include "xdata/xlightunit.h"
#include "xdata/xrootblock.h"
#include "xdata/xtable_bstate.h"
#include "xdata/xtableblock.h"
#include "xdata/xblockextract.h"
#include "xvledger/xreceiptid.h"
#include "xvledger/xvpropertyprove.h"
#include "xdata/xrelay_block.h"
#include "xdata/xblock_cs_para.h"

NS_BEG2(tcash, data)

class xblocktool_t {
 public:
    static base::xvblock_t*   create_genesis_empty_block(const std::string & account);
    static base::xvblock_t*   create_genesis_empty_unit(const std::string & account);
    static base::xvblock_t*   create_genesis_empty_table(const std::string & account);
    static base::xvblock_t*   create_genesis_lightunit(const std::string & account, int64_t tcash_balance);
    static base::xvblock_t*   create_genesis_lightunit(std::string const & account, chain_data::data_processor_t const & data);
    static base::xvblock_t*   create_genesis_lightunit(const std::string & account, const xtransaction_ptr_t & genesis_tx, const xtransaction_result_t & result);
    static base::xvblock_t*   create_genesis_lightunit(const xobject_ptr_t<base::xvbstate_t> & state, const xobject_ptr_t<base::xvcanvas_t> & canvas);
    static base::xvblock_t*   create_genesis_root_block(base::enum_xchain_id chainid, const std::string & account, const xrootblock_para_t & bodypara);
    static xrelay_block*      create_genesis_relay_block(const xrootblock_para_t & bodypara);
    static base::xvblock_t*   create_genesis_wrap_relayblock();

    static base::xvblock_t*   create_next_emptyblock(base::xvblock_t* prev_block, const xblock_consensus_para_t & cs_para);
    static base::xvblock_t*   create_next_emptyblock(base::xvblock_t* prev_block);
    static base::xvblock_t*   create_next_lightunit(const xlightunit_block_para_t & bodypara, base::xvblock_t* prev_block, const xblock_consensus_para_t & cs_para);
    static base::xvblock_t*   create_next_fullunit(const xfullunit_block_para_t & bodypara, base::xvblock_t* prev_block, const xblock_consensus_para_t & cs_para);
    static base::xvblock_t*   create_next_fulltable(const xfulltable_block_para_t & bodypara, base::xvblock_t* prev_block, const xblock_consensus_para_t & cs_para);

 public:
   //  static uint16_t         get_chain_id_from_account(const std::string & account);
    static std::string      make_address_table_account(base::enum_xchain_zone_index zone, uint16_t subaddr);
    static std::string      make_address_shard_table_account(uint16_t subaddr);
    static std::string      make_address_user_account(const std::string & public_key_address);
    static std::string      make_address_native_contract(base::enum_xchain_zone_index zone, const std::string & public_key_address, uint16_t subaddr);
    static std::string      make_address_shard_sys_account(const std::string & public_key_address, uint16_t subaddr);
    static std::string      make_address_zec_sys_account(const std::string & public_key_address, uint16_t subaddr);
    static std::string      make_address_beacon_sys_account(const std::string & public_key_address, uint16_t subaddr);
    // static std::string      make_address_user_contract(const std::string & public_key_address);

    static std::vector<std::string>     make_all_table_addresses();

 public:
    static bool             verify_latest_blocks(const base::xblock_mptrs & latest_blocks);
    static bool             verify_latest_blocks(base::xvblock_t* latest_cert_block, base::xvblock_t* lock_block, base::xvblock_t* commited_block);
    static bool             can_make_next_empty_block(const base::xblock_mptrs & latest_blocks, uint32_t max_empty_num);
    static bool             can_make_next_full_table(base::xvblock_t* latest_cert_block, uint32_t max_light_num);

 public:
    static bool             alloc_transaction_receiptid(const xcons_transaction_ptr_t & tx, base::xreceiptid_pair_t & receiptid_pair);

 public:  // txreceipt create
    static xcons_transaction_ptr_t                  create_one_txreceipt(base::xvblock_t* commit_block, base::xvblock_t* cert_block, base::xtable_shortid_t peer_table_sid, uint64_t receipt_id, enum_transaction_subtype subtype);
    static std::vector<xcons_transaction_ptr_t>     create_all_txreceipts(base::xvblock_t* commit_block, base::xvblock_t* cert_block);
    static std::vector<xcons_transaction_ptr_t>     create_txreceipts(base::xvblock_t* commit_block, base::xvblock_t* cert_block, const std::vector<xlightunit_action_t> & txactions);
    static std::vector<xlightunit_action_t>         unpack_all_txreceipt_action(base::xvblock_t* commit_block);
    static std::vector<xlightunit_action_t>         unpack_one_txreceipt_action(base::xvblock_t* commit_block, base::xtable_shortid_t peer_table_sid, uint64_t receipt_id, enum_transaction_subtype subtype);
 public:  // property prove    
    static base::xvproperty_prove_ptr_t             create_receiptid_property_prove(base::xvblock_t* commit_block, base::xvblock_t* cert_block, base::xvbstate_t* bstate);
    static base::xreceiptid_state_ptr_t             get_receiptid_from_property_prove(const base::xvproperty_prove_ptr_t & prop_prove);

   //  static bool get_receiptid_state_and_prove(base::xvblockstore_t * blockstore,
   //                                            const base::xvaccount_t & account,
   //                                            base::xvblock_t * latest_commit_block,
   //                                            base::xvproperty_prove_ptr_t & property_prove_ptr,
   //                                            xtablestate_ptr_t & tablestate_ptr);
};

// TODO(jimmy) xblocktool_t split to xblock_utl, xreceipt_utl, xaddress_utl


NS_END2
