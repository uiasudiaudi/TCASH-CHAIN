// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <map>
#include <vector>
#include "xbasic/xbyte_buffer.h"
#include "xbase/xobject_ptr.h"
#include "xbasic/xversion.h"
#include "xvledger/xdataobj_base.hpp"
#include "xvledger/xvaccount.h"
#include "xdata/xtransaction.h"
#include "xdata/xblock_paras.h"
#include "xdata/xblockaction.h"
#include "xdata/xethreceipt.h"
#include "xcommon/xtcash_log.h"
#include "xdata/xtcash_receipt.h"
namespace tcash { namespace data {

#define XTX_NOT_NEED_CONFIRM_FLAG_MASK (0x1)  // BIT#0
#define XTX_INNER_TABLE_FLAG_MASK (0x1 << 1)  // BIT#1

// the transaction state change after execute
class xtransaction_exec_state_t : public xblockpara_base_t {
 public:
    static XINLINE_CONSTEXPR char const * XPROPERTY_FEE_SEND_TX_LOCK_TGAS              = "0";
    static XINLINE_CONSTEXPR char const * XPROPERTY_FEE_RECV_TX_USE_SEND_TX_TGAS       = "1";
    static XINLINE_CONSTEXPR char const * XPROPERTY_FEE_TX_USED_TGAS                   = "2";
    static XINLINE_CONSTEXPR char const * XPROPERTY_FEE_TX_USED_DEPOSIT                = "3";
    static XINLINE_CONSTEXPR char const * XPROPERTY_FEE_TX_USED_DISK                   = "4";
    static XINLINE_CONSTEXPR char const * XTX_STATE_TX_EXEC_STATUS                     = "7";
    static XINLINE_CONSTEXPR char const * XTX_RECEIPT_ID                               = "8";
    static XINLINE_CONSTEXPR char const * XTX_RECEIPT_ID_SELF_TABLE_ID                 = "9";
    static XINLINE_CONSTEXPR char const * XTX_RECEIPT_ID_PEER_TABLE_ID                 = "a";
    static XINLINE_CONSTEXPR char const * XTX_SENDER_CONFRIMED_RECEIPT_ID              = "b";  // optional. exist when sendtx
    static XINLINE_CONSTEXPR char const * XTX_RECEIPT_DATA                             = "c";
    static XINLINE_CONSTEXPR char const * XTX_FLAGS                                    = "d";
    static XINLINE_CONSTEXPR char const * XTX_RSP_ID                                   = "e";
    static XINLINE_CONSTEXPR char const * XTX_EVM_TRANSACTION_RECEIPT                   = "f";
    static XINLINE_CONSTEXPR char const * XTX_TVM_TRANSACTION_RECEIPT                   = "g";

 public:
    xtransaction_exec_state_t();
    xtransaction_exec_state_t(const std::map<std::string, std::string> & values);

 public:
    void        set_used_disk(uint64_t value) {set_value(XPROPERTY_FEE_TX_USED_DISK, value);}
    void        set_used_tgas(uint64_t value) {set_value(XPROPERTY_FEE_TX_USED_TGAS, value);}
    void        set_used_deposit(uint64_t value) {set_value(XPROPERTY_FEE_TX_USED_DEPOSIT, value);}
    void        set_send_tx_lock_tgas(uint64_t value) {set_value(XPROPERTY_FEE_SEND_TX_LOCK_TGAS, value);}
    void        set_recv_tx_use_send_tx_tgas(uint64_t value) {set_value(XPROPERTY_FEE_RECV_TX_USE_SEND_TX_TGAS, value);}
    void        set_tx_exec_status(enum_xunit_tx_exec_status value);
    void        set_receipt_id(base::xtable_shortid_t self_tableid, base::xtable_shortid_t peer_tableid, uint64_t receiptid);
    void        set_sender_confirmed_receipt_id(uint64_t receiptid);
    void        set_receipt_data(xreceipt_data_t data);
    void        set_not_need_confirm(bool not_need_confirm);
    void        set_rsp_id(uint64_t rspid);
    void        set_inner_table_flag(bool inner_table);
    void        set_evm_tx_receipt(data::xeth_store_receipt_t & evm_tx_receipt);
    void        set_tvm_tx_receipt(data::xtcash_store_receipt_t & tvm_tx_receipt);

 public:
    uint64_t    get_used_disk()const {return get_value_uint64(XPROPERTY_FEE_TX_USED_DISK);}
    uint64_t    get_used_tgas()const {return get_value_uint64(XPROPERTY_FEE_TX_USED_TGAS);}
    uint64_t    get_used_deposit()const {return get_value_uint64(XPROPERTY_FEE_TX_USED_DEPOSIT);}
    uint64_t    get_send_tx_lock_tgas()const {return get_value_uint64(XPROPERTY_FEE_SEND_TX_LOCK_TGAS);}
    uint64_t    get_recv_tx_use_send_tx_tgas()const {return get_value_uint64(XPROPERTY_FEE_RECV_TX_USE_SEND_TX_TGAS);}
    enum_xunit_tx_exec_status   get_tx_exec_status() const;
    uint64_t    get_receipt_id()const {return get_value_uint64(XTX_RECEIPT_ID);}
    base::xtable_shortid_t    get_receipt_id_self_tableid()const {return get_value_uint16(XTX_RECEIPT_ID_SELF_TABLE_ID);}
    base::xtable_shortid_t    get_receipt_id_peer_tableid()const {return get_value_uint16(XTX_RECEIPT_ID_PEER_TABLE_ID);}
    bool                      get_not_need_confirm() const;
    uint64_t                  get_rsp_id()const {return get_value_uint64(XTX_RSP_ID);}
    bool                      get_inner_table_flag() const;
    bool                      get_evm_tx_receipt(data::xeth_store_receipt_t & evm_tx_receipt) const;
};

class xlightunit_tx_info_t : public xlightunit_action_t {
 public:
    xlightunit_tx_info_t(const base::xvaction_t & _action, xtransaction_t* raw_tx)
    : xlightunit_action_t(_action) {
        if (raw_tx != nullptr) {
            raw_tx->add_ref();
            m_raw_tx.attach(dynamic_cast<xtransaction_t*>(raw_tx));
        }
    }

 public:
    const xtransaction_ptr_t &  get_raw_tx() const {return m_raw_tx;}
    uint64_t                    get_last_trans_nonce() const;

 private:
    xtransaction_ptr_t          m_raw_tx;
};

using xlightunit_tx_info_ptr_t = std::shared_ptr<xlightunit_tx_info_t>;

}  // namespace data
}  // namespace tcash
