// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xblockaction.h"

#include "xdata/xdata_common.h"
#include "xdata/xlightunit_info.h"

#include <string>

namespace tcash { namespace data {

xlightunit_action_t::xlightunit_action_t(const base::xvaction_t & _action)
: base::xvaction_t(_action) {
    xassert(!_action.get_org_tx_hash().empty());
}

xlightunit_action_t::xlightunit_action_t(const std::string & tx_hash, base::enum_transaction_subtype _subtype, const std::string & caller_addr,const std::string & target_uri,const std::string & method_name)
: base::xvaction_t(tx_hash, caller_addr, target_uri, method_name) {
    set_org_tx_action_id(_subtype);
}

uint64_t xlightunit_action_t::get_used_disk()const {
    // TODO(jimmy) not support now
    return 0;
}
uint64_t xlightunit_action_t::get_used_tgas()const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XPROPERTY_FEE_TX_USED_TGAS);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}
uint64_t xlightunit_action_t::get_used_deposit()const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XPROPERTY_FEE_TX_USED_DEPOSIT);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}
uint64_t xlightunit_action_t::get_send_tx_lock_tgas()const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XPROPERTY_FEE_SEND_TX_LOCK_TGAS);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}
uint64_t xlightunit_action_t::get_recv_tx_use_send_tx_tgas()const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XPROPERTY_FEE_RECV_TX_USE_SEND_TX_TGAS);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}
enum_xunit_tx_exec_status xlightunit_action_t::get_tx_exec_status() const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_STATE_TX_EXEC_STATUS);
    if (!value.empty()) {
        return (enum_xunit_tx_exec_status)base::xstring_utl::touint32(value);
    }
    return enum_xunit_tx_exec_status_success;
}

bool xlightunit_action_t::get_evm_transaction_receipt(data::xeth_store_receipt_t & evm_tx_receipt) const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_EVM_TRANSACTION_RECEIPT);
    if (!value.empty()) {
        std::error_code ec;
        evm_tx_receipt.decodeBytes(to_bytes<std::string>(value), ec);
        if (ec) {
            xerror("xlightunit_action_t::get_evm_transaction_receipt decode fail");
            return false;
        }
        return true;
    }
    return false;
}

bool xlightunit_action_t::get_tvm_transaction_receipt(data::xtcash_store_receipt_t & tvm_tx_receipt) const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_TVM_TRANSACTION_RECEIPT);
    if (true == value.empty()) {
        return false;
    }
    std::error_code ec;
    tvm_tx_receipt.decodeBytes(to_bytes<std::string>(value), ec);
    if (ec) {
        xerror("[xlightunit_action_t::get_tvm_transaction_receipt] get-logs, decode fail");
        return false;
    }
    if (true == tvm_tx_receipt.get_logs().empty()){
        xwarn("[xlightunit_action_t::get_tvm_transaction_receipt] get-logs,the logs information is empty");
        return false;
    }
    tvm_tx_receipt.create_bloom();
    return true;
}

uint64_t xlightunit_action_t::get_receipt_id() const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_RECEIPT_ID);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}

uint64_t xlightunit_action_t::get_rsp_id() const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_RSP_ID);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}

base::xtable_shortid_t xlightunit_action_t::get_receipt_id_self_tableid()const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_RECEIPT_ID_SELF_TABLE_ID);
    if (!value.empty()) {
        return (base::xtable_shortid_t)base::xstring_utl::touint32(value);
    }
    return 0;
}

base::xtable_shortid_t xlightunit_action_t::get_receipt_id_peer_tableid()const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_RECEIPT_ID_PEER_TABLE_ID);
    if (!value.empty()) {
        return (base::xtable_shortid_t)base::xstring_utl::touint32(value);
    }
    return 0;
}

base::xtable_shortid_t xlightunit_action_t::get_rawtx_source_tableid() const {
    if (is_self_tx() || get_inner_table_flag()) {
        base::xvaccount_t _vaddr(get_caller());
        return _vaddr.get_short_table_id();
    } else if (is_send_tx() || is_confirm_tx()) {
        return get_receipt_id_self_tableid();
    } else {
        return get_receipt_id_peer_tableid();
    }
}

uint64_t xlightunit_action_t::get_sender_confirmed_receipt_id() const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_SENDER_CONFRIMED_RECEIPT_ID);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}

bool xlightunit_action_t::get_not_need_confirm() const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_FLAGS);
    if (!value.empty()) {
        auto flags = base::xstring_utl::touint32(value);
        return flags & XTX_NOT_NEED_CONFIRM_FLAG_MASK;
    }
    return false;
}

bool xlightunit_action_t::get_inner_table_flag() const {
    std::string value = get_action_result_property(xtransaction_exec_state_t::XTX_FLAGS);
    if (!value.empty()) {
        auto flags = base::xstring_utl::touint32(value);
        return flags & XTX_INNER_TABLE_FLAG_MASK;
    }
    return false;
}

bool xlightunit_action_t::is_need_make_txreceipt() const {
    if (is_txaction()) {
        if (is_send_tx()) {
            if (!get_inner_table_flag()) {
                return true;
            }
        } else if (is_recv_tx()) {
            if (!get_not_need_confirm()) {
                return true;
            }
        }
    }

    return false;
}

std::string xlightunit_action_t::get_action_result_property(const std::string & key) const {
    const std::map<std::string,std::string>* map_ptr = get_method_result()->get_map<std::string>();
    if (map_ptr != nullptr) {
        auto iter = map_ptr->find(key);
        if (iter != map_ptr->end()) {
            return iter->second;
        }
        return {};
    }
    xassert(false);
    return {};
}



//----------------------------------------xtableblock_action_t-------------------------------------//
xtableblock_action_t::xtableblock_action_t(const base::xvaction_t & _action)
: base::xvaction_t(_action) {
}

xtableblock_action_t::xtableblock_action_t(const std::string & target_uri, const std::map<std::string, std::string> & property_hashs, base::xtable_shortid_t tableid, uint64_t height)
: base::xvaction_t(std::string(), std::string(), target_uri, std::string("b")) {
    std::map<std::string, std::string> action_result = property_hashs;
    action_result[XPROPERTY_SELF_TABLE_ID] = std::to_string(tableid);
    action_result[XPROPERTY_SELF_TABLE_HEIGHT] = std::to_string(height);
    base::xvalue_t _action_result(action_result);
    base::xvaction_t::copy_result(_action_result);
}

base::xtable_shortid_t xtableblock_action_t::get_self_tableid() const {
    std::string value = get_action_result_property(XPROPERTY_SELF_TABLE_ID);
    if (!value.empty()) {
        return static_cast<uint16_t>(base::xstring_utl::touint32(value));
    }
    return 0;
}

uint64_t xtableblock_action_t::get_self_table_height() const {
    std::string value = get_action_result_property(XPROPERTY_SELF_TABLE_HEIGHT);
    if (!value.empty()) {
        return base::xstring_utl::touint64(value);
    }
    return 0;
}

std::string xtableblock_action_t::get_property_hash(const std::string & propname) const {
    return get_action_result_property(propname);
}

std::string xtableblock_action_t::get_action_result_property(const std::string & key) const {
    const std::map<std::string,std::string>* map_ptr = get_method_result()->get_map<std::string>();
    if (map_ptr != nullptr) {
        auto iter = map_ptr->find(key);
        if (iter != map_ptr->end()) {
            return iter->second;
        }
        return {};
    }
    xassert(false);
    return {};
}

}  // namespace data
}  // namespace tcash
