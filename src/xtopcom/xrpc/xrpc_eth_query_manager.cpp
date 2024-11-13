#include "xrpc_eth_query_manager.h"

#include "xbase/xbase.h"
#include "xbase/xcontext.h"
#include "xbase/xint.h"
#include "xbase/xutl.h"
#include "xbasic/xfixed_hash.h"
#include "xbasic/xhex.h"
#include "xbasic/xutility.h"
#include "xcodec/xmsgpack_codec.hpp"
#include "xcommon/common.h"
#include "xcommon/common_data.h"
#include "xcommon/xerror/xerror.h"
#include "xconfig/xconfig_register.h"
#include "xdata/xblock_cs_para.h"
#include "xdata/xblocktool.h"
#include "xdata/xproposal_data.h"
#include "xdata/xrelay_block.h"
#include "xdata/xrelay_block_store.h"
#include "xdata/xtable_bstate.h"
#include "xdata/xtransaction_v2.h"
#include "xdata/xtransaction_v3.h"
#include "xdata/xtx_factory.h"
#include "xdata/xverifier/xverifier_utl.h"
#include "xevm/xevm.h"
#include "xmbus/xevent_behind.h"
#include "xpbase/base/tcash_utils.h"
#include "xrpc/eth_rpc/eth_error_code.h"
#include "xrpc/xrpc_eth_parser.h"
#include "xrpc/xrpc_loader.h"
#include "xrpc/xuint_format.h"
#include "xstatectx/xstatectx.h"
#include "xstatestore/xstatestore_face.h"
#include "xtxexecutor/xtransaction_fee.h"
#include "xtxexecutor/xvm_face.h"
#include "xvledger/xvblock.h"
#include "xvledger/xvledger.h"
#include "xvm/manager/xcontract_manager.h"
#include "xgasfee/xgas_tx_operator.h"
#include "xgasfee/xgas_estimate.h"

#include <algorithm>
#include <cstdint>
#if defined(XCXX20)
#    include <secp256k1.h>
#else
#    include <secp256k1/secp256k1.h>
#endif

using namespace tcash::data;
using namespace tcash::base;
using namespace tcash::store;
using namespace tcash::xrpc;

// Error(message)
static constexpr uint32_t abi_error = 0x08c379a0;

namespace tcash {
namespace xrpc {

#define ETH_ADDRESS_CHECK_VALID(x)                                                                                                                                                 \
    if (xverifier::xtx_utl::address_is_valid(x) != xverifier::xverifier_error::xverifier_success) {                                                                                \
        xwarn("xtx_verifier address_verify address invalid, account:%s", x.c_str());                                                                                               \
        strResult = "account address is invalid";                                                                                                                                  \
        nErrorCode = (uint32_t)enum_xrpc_error_code::rpc_param_param_error;                                                                                                        \
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_address, "invalid address");                                                                               \
        return;                                                                                                                                                                    \
    }

#define REGISTER_ETH_QUERY_METHOD(func_name)                                                                                                                                       \
    m_query_method_map.emplace(std::pair<std::string, tcash::xrpc::query_method_handler>{                                                                                            \
        std::string{#func_name}, std::bind(&xrpc_eth_query_manager::func_name, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)})

xrpc_eth_query_manager::xrpc_eth_query_manager(observer_ptr<base::xvblockstore_t> block_store) : m_block_store(block_store) {
    REGISTER_ETH_QUERY_METHOD(eth_getBalance);
    REGISTER_ETH_QUERY_METHOD(eth_getTransactionByHash);
    REGISTER_ETH_QUERY_METHOD(eth_getTransactionReceipt);
    REGISTER_ETH_QUERY_METHOD(eth_getTransactionCount);
    REGISTER_ETH_QUERY_METHOD(eth_blockNumber);
    REGISTER_ETH_QUERY_METHOD(eth_getBlockByHash);
    REGISTER_ETH_QUERY_METHOD(eth_getBlockByNumber);
    REGISTER_ETH_QUERY_METHOD(eth_getCode);
    REGISTER_ETH_QUERY_METHOD(eth_call);
    REGISTER_ETH_QUERY_METHOD(eth_estimateGas);
    REGISTER_ETH_QUERY_METHOD(eth_getStorageAt);
    REGISTER_ETH_QUERY_METHOD(eth_getLogs);
    REGISTER_ETH_QUERY_METHOD(eth_feeHistory);

    REGISTER_ETH_QUERY_METHOD(tcashRelay_getBlockByNumber);
    REGISTER_ETH_QUERY_METHOD(tcashRelay_getBlockByHash);
    REGISTER_ETH_QUERY_METHOD(tcashRelay_blockNumber);
    REGISTER_ETH_QUERY_METHOD(tcashRelay_getTransactionByHash);
    REGISTER_ETH_QUERY_METHOD(tcashRelay_getTransactionReceipt);

    REGISTER_ETH_QUERY_METHOD(tcash_getBalance);
}

void xrpc_eth_query_manager::call_method(std::string strMethod, Json::Value & js_req, Json::Value & js_rsp, std::string & strResult, uint32_t & nErrorCode) {
    auto const iter = m_query_method_map.find(strMethod);
    if (iter != m_query_method_map.end()) {
        (iter->second)(js_req, js_rsp, strResult, nErrorCode);
    }
}

bool xrpc_eth_query_manager::handle(std::string & strReq, Json::Value & js_req, Json::Value & js_rsp, std::string & strResult, uint32_t & nErrorCode) {
    std::string action = js_req["action"].asString();
    auto const iter = m_query_method_map.find(action);
    if (iter != m_query_method_map.end()) {
        iter->second(js_req, js_rsp, strResult, nErrorCode);
    } else {
        xinfo("get_block action:%s not exist!", action.c_str());
        strResult = "Method not Found!";
        return false;
    }

    return true;
}

enum_query_result xrpc_eth_query_manager::query_account_by_number(const std::string &unit_address, const std::string& table_height, data::xunitstate_ptr_t& ptr) {
    ptr = statestore::xstatestore_hub_t::instance()->get_unit_state_by_table(common::xaccount_address_t(unit_address), table_height);
    XMETRICS_GAUGE(metrics::rpc_query_account_succ, nullptr != ptr ? 1 : 0);
    if (nullptr == ptr) {
        xwarn("xstore::query_account_by_number fail-load. account=%s,table_height=%s", unit_address.c_str(),table_height.c_str());
        return enum_block_not_found;        
    }    
    return enum_success;
}
xobject_ptr_t<base::xvblock_t> xrpc_eth_query_manager::query_block_by_height(const std::string& table_height) {
    xdbg("xrpc_eth_query_manager::query_block_by_height: %s, %s",  common::eth_table_base_address.to_string().c_str(), table_height.c_str());
    auto const table_addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).vaccount();

    xobject_ptr_t<base::xvblock_t> block;
    if (table_height == BlockHeightLatest)
        block = base::xvchain_t::instance().get_xblockstore()->get_latest_cert_block(table_addr);
    else if (table_height == BlockHeightEarliest)
        block = base::xvchain_t::instance().get_xblockstore()->get_genesis_block(table_addr);
    else if (table_height == BlockHeightPending)
        block = base::xvchain_t::instance().get_xblockstore()->get_latest_cert_block(table_addr);
    else {
        uint64_t const height = std::strtoul(table_height.c_str(), nullptr, 16);
        block = m_block_store->load_block_object(table_addr, height, base::enum_xvblock_flag_authenticated, false);
    }
    return block;
}
uint64_t xrpc_eth_query_manager::get_block_height(const std::string& table_height) {
    uint64_t height = 0;
    std::string addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).to_string();
    base::xvaccount_t _vaddress(addr);
    uint64_t max_height = m_block_store->get_latest_cert_block_height(_vaddress);

    if (table_height == BlockHeightLatest)
        height = max_height;
    else if (table_height == BlockHeightEarliest)
        height = 0;
    else if (table_height == BlockHeightPending) {
        height = max_height;
    } else {
        height = std::strtoul(table_height.c_str(), nullptr, 16);
        if (height > max_height)
            height = max_height;
    }
    return height;
}
void xrpc_eth_query_manager::eth_getBalance(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_address))
        return;
    if (!eth::EthErrorCode::check_eth_address(js_req[0].asString(), js_rsp))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_block))
        return;

    std::string account = js_req[0].asString();
    account = xvaccount_t::to_evm_address(account);
    xdbg("xrpc_eth_query_manager::eth_getBalance account: %s,%s", account.c_str(), js_req[1].asString().c_str());

    ETH_ADDRESS_CHECK_VALID(account)

    data::xunitstate_ptr_t account_ptr;
    enum_query_result ret = query_account_by_number(account, js_req[1].asString(), account_ptr);

    if (ret == enum_block_not_found) {
        std::string msg = "header not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    } else if (ret == enum_unit_not_found) {
        js_rsp["result"] = "0x0";
    } else if (ret == enum_success) {
        auto default_token_type = XGET_CONFIG(evm_token_type);
        evm_common::u256 balance = 0;
        xdbg("xrpc_eth_query_manager::eth_getBalance token type is %s.", default_token_type.c_str());

        if (default_token_type == "tcash") {
            balance = account_ptr->balance();
        } else if(default_token_type == "ETH") {
            evm_common::u256 tcash_balance = account_ptr->balance();
            evm_common::u256 eth_balance = account_ptr->tep_token_balance(common::xtoken_id_t::eth);
            balance = tcash::gasfee::xgas_estimate::get_nominal_balance(tcash_balance, eth_balance);
        }

        js_rsp["result"] = xrpc_eth_parser_t::u256_to_hex_prefixed(balance);
        xdbg("xrpc_eth_query_manager::eth_getBalance address=%s,balance=%s,%s", account.c_str(), balance.str().c_str(), xrpc_eth_parser_t::u256_to_hex_prefixed(balance).c_str());
    }
}
void xrpc_eth_query_manager::eth_getTransactionCount(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_address))
        return;
    if (!eth::EthErrorCode::check_eth_address(js_req[0].asString(), js_rsp))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_block))
        return;

    std::string account = js_req[0].asString();
    account = xvaccount_t::to_evm_address(account);

    // add tcash address check
    ETH_ADDRESS_CHECK_VALID(account)
    base::xaccount_index_t accountindex;
    if (false == statestore::xstatestore_hub_t::instance()->get_accountindex(js_req[1].asString(), common::xaccount_address_t(account), accountindex)) {
        xwarn("xrpc_eth_query_manager::eth_getTransactionCount fail-load. account=%s,table_height=%s", account.c_str(),js_req[1].asString().c_str());
        std::string msg = "header not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    }

    uint64_t nonce = accountindex.get_latest_tx_nonce();
    xdbg("xarc_query_manager::eth_getTransactionCount: %s, %llu", account.c_str(), nonce);
    std::stringstream outstr;
    outstr << "0x" << std::hex << nonce;
    js_rsp["result"] = std::string(outstr.str());
}
void xrpc_eth_query_manager::eth_getTransactionByHash(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 1))
        return;
    std::string tx_hash = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(tx_hash, js_rsp, 0, eth::enum_rpc_type_hash))
        return;
    if (!eth::EthErrorCode::check_hash(tx_hash, js_rsp))
        return;

    uint256_t hash = tcash::data::hex_to_uint256(js_req[0].asString());
    std::string tx_hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
    xdbg("eth_getTransactionByHash tx hash: %s",  tx_hash.c_str());

    xtxindex_detail_ptr_t sendindex = xrpc_loader_t::load_ethtx_indx_detail(tx_hash_str);
    if (sendindex == nullptr) {
        xwarn("xrpc_eth_query_manager::eth_getTransactionByHash fail.tx hash:%s", tx_hash.c_str());
        js_rsp["result"] = Json::Value::null;
        return;
    }

    std::string block_hash = tcash::to_hex_prefixed(sendindex->get_block_hash());
    std::string block_num = xrpc_eth_parser_t::uint64_to_hex_prefixed(sendindex->get_txindex()->get_block_height());
    std::string tx_index = xrpc_eth_parser_t::uint64_to_hex_prefixed(sendindex->get_transaction_index());
    xtx_location_t txlocation(block_hash, block_num, tx_hash, tx_index);

    Json::Value js_result;
    std::error_code ec;
    xrpc_eth_parser_t::transaction_to_json(txlocation, sendindex->get_raw_tx(), js_result, ec);
    if (ec) {
        xerror("xrpc_eth_query_manager::eth_getTransactionByHash fail-transaction_to_json.tx hash:%s", tx_hash.c_str());
        js_rsp["result"] = Json::Value::null;
        return;
    }
    js_rsp["result"] = js_result;
    xdbg("xrpc_eth_query_manager::eth_getTransactionByHash ok.tx hash:%s", tx_hash.c_str());
    return;
}

void xrpc_eth_query_manager::eth_getTransactionReceipt(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 1))
        return;
    std::string tx_hash = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(tx_hash, js_rsp, 0, eth::enum_rpc_type_hash))
        return;
    if (!eth::EthErrorCode::check_hash(tx_hash, js_rsp))
        return;

    std::error_code ec;
    auto tx_hash_bytes = tcash::from_hex(tx_hash, ec);
    if (ec) {
        xdbg("eth_getTransactionReceipt from_hex fail: %s",  tx_hash.c_str());
        return;
    }
    std::string raw_tx_hash = tcash::to_string(tx_hash_bytes);
    xdbg("eth_getTransactionReceipt tx hash: %s",  tx_hash.c_str());

    xtxindex_detail_ptr_t sendindex = xrpc_loader_t::load_ethtx_indx_detail(raw_tx_hash);
    if (sendindex == nullptr) {
        xwarn("xrpc_query_manager::eth_getTransactionReceipt load tx index fail.%s", tx_hash.c_str());
        nErrorCode = (uint32_t)enum_xrpc_error_code::rpc_shard_exec_error;
        js_rsp["result"] = Json::Value::null;
        return;
    }

    Json::Value js_result;    
    xrpc_eth_parser_t::receipt_to_json(tx_hash, sendindex, js_result, ec);
    if (ec) {
        xerror("xrpc_query_manager::eth_getTransactionReceipt parse json fail.%s", tx_hash.c_str());
        nErrorCode = (uint32_t)enum_xrpc_error_code::rpc_shard_exec_error;
        js_rsp["result"] = Json::Value::null;
        return;
    }
    js_rsp["result"] = js_result;
    xdbg("xrpc_query_manager::eth_getTransactionReceipt ok.tx hash:%s", js_req[0].asString().c_str());
    return;
}
void xrpc_eth_query_manager::eth_blockNumber(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 0))
        return;
    std::string addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).to_string();
    base::xvaccount_t _vaddress(addr);
    uint64_t height = m_block_store->get_latest_cert_block_height(_vaddress);

    std::stringstream outstr;
    outstr << "0x" << std::hex << height;
    js_rsp["result"] = std::string(outstr.str());
    xinfo("xarc_query_manager::eth_blockNumber: %llu", height);
}

void xrpc_eth_query_manager::eth_getBlockByHash(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    std::string tx_hash = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(tx_hash, js_rsp, 0, eth::enum_rpc_type_hash))
        return;
    if (!eth::EthErrorCode::check_hash(tx_hash, js_rsp))
        return;
    if (!js_req[1].isBool()) {
        std::string msg = "parse error";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
        return;
    }

    uint256_t hash = tcash::data::hex_to_uint256(js_req[0].asString());
    std::string block_hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
    xdbg("eth_getBlockByHash block hash: %s",  tcash::HexEncode(block_hash_str).c_str());

    base::xauto_ptr<base::xvblock_t>  block = m_block_store->get_block_by_hash(block_hash_str);
    if (block == nullptr) {
        js_rsp["result"] = Json::Value::null;
        return;
    }

    std::error_code ec;
    Json::Value js_result;
    set_block_result(block, js_result, js_req[1].asBool(), ec);
    js_rsp["result"] = js_result;
    return;
}
void xrpc_eth_query_manager::eth_getBlockByNumber(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_block))
        return;
    if (!js_req[1].isBool()) {
        std::string msg = "parse error";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
        return;
    }

    xobject_ptr_t<base::xvblock_t>  block = query_block_by_height(js_req[0].asString());
    if (block == nullptr) {
        js_rsp["result"] = Json::Value::null;
        return;
    }

    std::error_code ec;
    Json::Value js_result;
    set_block_result(block, js_result, js_req[1].asBool(), ec);
    js_rsp["result"] = js_result;
    return;
}
void xrpc_eth_query_manager::set_block_result(const xobject_ptr_t<base::xvblock_t>&  block, Json::Value& js_result, bool fullTx, std::error_code & ec) {
    base::xvaccount_t _vaddress(block->get_account());
    if (!_vaddress.is_table_address()) {
        ec = common::error::xerrc_t::invalid_db_load;
        xwarn("xrpc_eth_query_manager::set_block_result,fail invalid input for block:%s", block->dump().c_str());
        return;
    }

    // TODO(jimmy) block size need load input and output. transactions hash need load input
    if (block->get_block_class() != base::enum_xvblock_class_nil) {
        if (false == base::xvchain_t::instance().get_xblockstore()->load_block_input(_vaddress, block.get())) {
            ec = common::error::xerrc_t::invalid_db_load;
            xerror("xrpc_eth_query_manager::set_block_result,fail to load block input for block:%s", block->dump().c_str());
            return;
        }
        if (false == base::xvchain_t::instance().get_xblockstore()->load_block_output(_vaddress, block.get())) {
            ec = common::error::xerrc_t::invalid_db_load;
            xerror("xrpc_eth_query_manager::set_block_result,fail to load block output for block:%s", block->dump().c_str());
            return;
        }
    }

    xrpc_eth_parser_t::blockheader_to_json(block.get(), js_result, ec);
    if (ec) {
        return;
    }

    std::string block_hash = tcash::to_hex_prefixed(block->get_block_hash());
    std::string block_num = xrpc_eth_parser_t::uint64_to_hex_prefixed(block->get_height());

    auto input_actions = data::xblockextract_t::unpack_eth_txactions(block.get());
    for (uint64_t txindex = 0; txindex < (uint64_t)input_actions.size(); txindex++) {
        auto & action = input_actions[txindex];
        std::string tx_hash = tcash::to_hex_prefixed(action.get_org_tx_hash());
        if (!fullTx) {
            js_result["transactions"].append(tx_hash);
        } else {
            data::xtransaction_ptr_t raw_tx = data::xblockextract_t::unpack_raw_tx(block.get(), action.get_org_tx_hash(), ec);
            if (raw_tx == nullptr) {
                xerror("xrpc_eth_query_manager::set_block_result fail.tx hash:%s", to_hex_str(action.get_org_tx_hash()).c_str());
                continue;
            }

            xtx_location_t txlocation(block_hash, block_num, tx_hash, xrpc_eth_parser_t::uint64_to_hex_prefixed(txindex));
            Json::Value js_tx;
            xrpc_eth_parser_t::transaction_to_json(txlocation, raw_tx, js_tx, ec);
            if (ec) {
                xerror("xrpc_eth_query_manager::set_block_result fail-transaction_to_json.tx hash:%s", to_hex_str(action.get_org_tx_hash()).c_str());
                continue;
            }
            js_result["transactions"].append(js_tx);
        }
    }    
}
void xrpc_eth_query_manager::eth_getCode(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_address))
        return;
    if (!eth::EthErrorCode::check_eth_address(js_req[0].asString(), js_rsp))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_block))
        return;

    std::string account = js_req[0].asString();
    account = xvaccount_t::to_evm_address(account);

    // add tcash address check
    ETH_ADDRESS_CHECK_VALID(account)

    data::xunitstate_ptr_t account_ptr;
    enum_query_result ret = query_account_by_number(account, js_req[1].asString(), account_ptr);
    if (ret == enum_block_not_found) {
        std::string msg = "header not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    } else if (ret == enum_unit_not_found) {
        js_rsp["result"] = "0x0";
        return;
    } else if (ret == enum_success) {
        std::string code = account_ptr->get_code();
        code = std::string("0x") + tcash::HexEncode(code);
        xdbg("xrpc_eth_query_manager::eth_getCode account: %s, %s", account.c_str(), code.c_str());
        js_rsp["result"] = code;
    }
    return;
}
std::string xrpc_eth_query_manager::safe_get_json_value(Json::Value & js_req, const std::string& key) {
    if (js_req.isMember(key))
        return js_req[key].asString();
    return "";
}
void xrpc_eth_query_manager::eth_call(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_block))
        return;

    std::string to = safe_get_json_value(js_req[0], "to");
    if (!eth::EthErrorCode::check_eth_address(to, js_rsp))
        return;
    to = xvaccount_t::to_evm_address(to);
    ETH_ADDRESS_CHECK_VALID(to)

    std::string from = safe_get_json_value(js_req[0], "from");
    if (from.empty())
        from = std::string("0x") + std::string(40, '0');
    if (!eth::EthErrorCode::check_eth_address(from, js_rsp))
        return;

    from = xvaccount_t::to_evm_address(from);
    ETH_ADDRESS_CHECK_VALID(from)

    std::string data;
    std::string jdata = safe_get_json_value(js_req[0], "input");
    if (jdata.empty()) {
        jdata = safe_get_json_value(js_req[0], "data");
    }
    if (jdata.empty()) {
        js_rsp["result"] = "0x";
        return;
    } else {
        if (!eth::EthErrorCode::check_hex(jdata, js_rsp, 0, eth::enum_rpc_type_data))
            return;
        data = tcash::HexDecode(jdata.substr(2));
    }

    std::string value = safe_get_json_value(js_req[0], "value");
    std::string gas = safe_get_json_value(js_req[0], "gas");
    std::string gas_price = safe_get_json_value(js_req[0], "gasPrice");
    if (!gas.empty()) {
        if (!eth::EthErrorCode::check_hex(gas, js_rsp, 0, eth::enum_rpc_type_unknown))
            return;
    }
    if (!gas_price.empty()) {
        if (!eth::EthErrorCode::check_hex(gas_price, js_rsp, 0, eth::enum_rpc_type_unknown))
            return;
    }

    evm_common::u256 gas_u256{0};
    evm_common::u256 gas_price_u256{0};
    if (!gas.empty()) {
        if (!eth::EthErrorCode::check_hex(gas, js_rsp, 0, eth::enum_rpc_type_unknown)) {
            return;
        }
        gas_u256 = evm_common::fromBigEndian<evm_common::u256>(gas);
    } else {
        gas_u256 = evm_common::u256(XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit));
    }
    if (!gas_price.empty()) {
        if (!eth::EthErrorCode::check_hex(gas_price, js_rsp, 0, eth::enum_rpc_type_unknown)) {
            return;
        } else {
            gas_price_u256 = evm_common::fromBigEndian<evm_common::u256>(gas_price);
        }
    }

    tcash::data::xtransaction_ptr_t tx = tcash::data::xtx_factory::create_ethcall_v3_tx(from, to, data, std::strtoul(value.c_str(), NULL, 16), gas_u256, gas_price_u256);
    auto cons_tx = tcash::make_object_ptr<tcash::data::xcons_transaction_t>(tx.get());
    xinfo("xrpc_eth_query_manager::eth_call, %s, %s, %s", jdata.c_str(), value.c_str(), gas_u256.str().c_str());

    std::string addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).to_string();
    base::xvaccount_t _vaddress(addr);
    //auto block = m_block_store->get_latest_committed_block(_vaddress);   
    xobject_ptr_t<base::xvblock_t> block = query_block_by_height(js_req[1].asString());
    if (block == nullptr) {
        std::string msg = "header not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    }
    uint64_t gmtime = block->get_second_level_gmtime();
    xblock_consensus_para_t cs_para(addr, block->get_clock(), block->get_viewid(), block->get_viewtoken(), block->get_height() + 1, gmtime);

    // TODO(jimmy) should create statectx by block
    statectx::xstatectx_ptr_t statectx_ptr = statectx::xstatectx_factory_t::create_statectx(_vaddress, block.get());
    if (statectx_ptr == nullptr) {
        xwarn("create_statectx fail: %s", addr.c_str());
        std::string msg = "err: statectx create fail";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    }

    common::xaccount_address_t _vaddr(from);
    data::xunitstate_ptr_t unitstate = statectx_ptr->load_unit_state(_vaddr);
    if (nullptr == unitstate) {
        xwarn("eth_call fail-load unit state, %s", from.c_str());
        std::string msg = "err: unit state load fail";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);        
        return;
    }

    uint64_t gas_limit = XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit);
    txexecutor::xvm_para_t vmpara(cs_para.get_clock(), cs_para.get_random_seed(), cs_para.get_total_lock_tgas_token(), gas_limit, cs_para.get_table_proposal_height(), eth_miner_zero_address);
    txexecutor::xvm_input_t input{statectx_ptr, vmpara, cons_tx};
    txexecutor::xvm_output_t output;
    tcash::evm::xtcash_evm evm{tcash::make_observer(contract_runtime::evm::xevm_contract_manager_t::instance()), statectx_ptr};

    auto ret = evm.execute(input, output);
    if (ret != txexecutor::enum_exec_success) {
        xwarn("evm call fail.");
        std::string msg = "err: evm execute fail " + std::to_string(ret);
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);        
        return;
    }
    xinfo("evm call: %d, %s", output.m_tx_result.status, output.m_tx_result.extra_msg.c_str());
    if (output.m_tx_result.status == evm_common::OutOfFund) {
        std::string msg = "insufficient funds for transfer";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    } else if (output.m_tx_result.status == evm_common::OutOfGas) {
        std::string msg = std::string("err: intrinsic gas too low: have ") + gas_u256.str() + ", want " + std::to_string(output.m_tx_result.used_gas)
            + " (supplied gas " + gas_u256.str() + ")";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    } else if (output.m_tx_result.status == evm_common::Success) {
        if (output.m_tx_result.extra_msg.empty())
            output.m_tx_result.extra_msg = "0x";
        js_rsp["result"] = output.m_tx_result.extra_msg;
    } else if (output.m_tx_result.status == evm_common::Revert) {
        auto const & extra_msg = output.m_tx_result.extra_msg;
        js_rsp["error"]["message"] = "execution reverted";
        js_rsp["error"]["code"] = eth::enum_eth_rpc_execution_reverted;
        js_rsp["error"]["data"] = extra_msg;
        if (false == extra_msg.empty() && extra_msg != "0x"){
            auto t = evm_common::xabi_decoder_t::build_from_hex_string(extra_msg);
            auto selector = t.extract<evm_common::xfunction_selector_t>();
            if (selector.method_id == abi_error) {
                js_rsp["error"]["message"] = "execution reverted: " + t.extract<std::string>();
                js_rsp["error"]["data"] = extra_msg;
            }
        } 
    }else {
        js_rsp["error"]["code"] = eth::enum_eth_rpc_default_error;
        js_rsp["error"]["message"] = "execution reverted";
    }
}

void xrpc_eth_query_manager::eth_estimateGas(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    std::string block_number = BlockHeightLatest;
    if (js_req.size() >= 2)
        block_number = js_req[1].asString();
    if (!eth::EthErrorCode::check_hex(block_number, js_rsp, 1, eth::enum_rpc_type_block))
        return;

    std::string to = safe_get_json_value(js_req[0], "to");
    if (to.empty()) {
        to = std::string("0x") + std::string(40, '0');
    } else {
        if (!eth::EthErrorCode::check_eth_address(to, js_rsp))
            return;
    }
    to = xvaccount_t::to_evm_address(to);
    xdbg("xrpc_eth_query_manager::eth_estimateGas, to: %s", to.c_str());
    ETH_ADDRESS_CHECK_VALID(to)

    std::string from = safe_get_json_value(js_req[0], "from");
    if (from.empty())
        from = std::string("0x") + std::string(40, '0');
    if (!eth::EthErrorCode::check_eth_address(from, js_rsp))
        return;

    from = xvaccount_t::to_evm_address(from);
    xdbg("xrpc_eth_query_manager::eth_estimateGas, from: %s", from.c_str());
    ETH_ADDRESS_CHECK_VALID(from)

    std::string data;
    std::string jdata = safe_get_json_value(js_req[0], "input");
    if (jdata.empty()) {
        jdata = safe_get_json_value(js_req[0], "data");
    }
    if (jdata.empty()) {
        js_rsp["result"] = "0x5208";
        return;
    } else {
        if (!eth::EthErrorCode::check_hex(jdata, js_rsp, 0, eth::enum_rpc_type_data))
            return;
        data = tcash::HexDecode(jdata.substr(2));
    }

    std::string value = safe_get_json_value(js_req[0], "value");
    std::string gas = safe_get_json_value(js_req[0], "gas");
    std::string gas_price = safe_get_json_value(js_req[0], "gasPrice");

    evm_common::u256 gas_u256{0};
    evm_common::u256 gas_price_u256{0};
    if (!gas.empty()) {
        if (!eth::EthErrorCode::check_hex(gas, js_rsp, 0, eth::enum_rpc_type_unknown)) {
            return;
        }
        gas_u256 = evm_common::fromBigEndian<evm_common::u256>(gas);
        if (gas_u256 > evm_common::u256(XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit))) {
            gas_u256 = evm_common::u256(XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit));
        }
    } else {
        gas_u256 = evm_common::u256(XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit));
    }
    if (!gas_price.empty()) {
        if (!eth::EthErrorCode::check_hex(gas_price, js_rsp, 0, eth::enum_rpc_type_unknown)) {
            return;
        } else {
            gas_price_u256 = evm_common::fromBigEndian<evm_common::u256>(gas_price);
        }
    }

    tcash::data::xtransaction_ptr_t tx = tcash::data::xtx_factory::create_ethcall_v3_tx(from, to, data, std::strtoul(value.c_str(), NULL, 16), gas_u256, gas_price_u256);
    auto cons_tx = tcash::make_object_ptr<tcash::data::xcons_transaction_t>(tx.get());
    xinfo("xrpc_eth_query_manager::eth_estimateGas, %s, %s, %s", jdata.c_str(), value.c_str(), gas_u256.str().c_str());

    std::string addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).to_string();
    base::xvaccount_t _vaddress(addr);
    xobject_ptr_t<base::xvblock_t> block = query_block_by_height(block_number);
    if (block == nullptr) {
        std::string msg = "block not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    }
    uint64_t gmtime = block->get_second_level_gmtime();
    xblock_consensus_para_t cs_para(addr, block->get_clock(), block->get_viewid(), block->get_viewtoken(), block->get_height() + 1, gmtime);

    statectx::xstatectx_ptr_t statectx_ptr = statectx::xstatectx_factory_t::create_statectx(_vaddress, block.get());
    if (statectx_ptr == nullptr) {
        xwarn("create statectx fail: %s", addr.c_str());
        std::string msg = "err: statectx create fail";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);        
        return;
    }

    common::xaccount_address_t _vaddr(from);
    data::xunitstate_ptr_t unitstate = statectx_ptr->load_unit_state(_vaddr);
    if (nullptr == unitstate) {
        xwarn("eth_estimateGas fail-load unit state, %s", from.c_str());
        std::string msg = "err: unit state load fail";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);              
        return;
    }

    uint64_t gas_limit = XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit);
    txexecutor::xvm_para_t vmpara(cs_para.get_clock(), cs_para.get_random_seed(), cs_para.get_total_lock_tgas_token(), gas_limit, cs_para.get_table_proposal_height(), eth_miner_zero_address);

    txexecutor::xvm_input_t input{statectx_ptr, vmpara, cons_tx};
    txexecutor::xvm_output_t output;
    tcash::evm::xtcash_evm evm{tcash::make_observer(contract_runtime::evm::xevm_contract_manager_t::instance()), statectx_ptr};

    auto ret = evm.execute(input, output);
    if (ret != txexecutor::enum_exec_success) {
        xwarn("evm call fail.");
        std::string msg = "err: evm execute fail " + std::to_string(ret);
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);              
        return;
    }
    auto const & extra_msg = output.m_tx_result.extra_msg;
    xinfo("eth_estimateGas call: %d, %d, %s, %llu", ret, output.m_tx_result.status, extra_msg.c_str(), output.m_tx_result.used_gas);

    switch (output.m_tx_result.status) {
    case evm_common::Success: {
        std::stringstream outstr;
        outstr << "0x" << std::hex << (uint64_t)(output.m_tx_result.used_gas * 1.5); // TODO(jimmy) always mutiply 1.2 avoiding fail of some cases
        js_rsp["result"] = outstr.str();
        break;
    }

    case evm_common::OutOfFund: {
        std::string msg = "insufficient funds for transfer";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        break;
    }

    case evm_common::Revert: {
        js_rsp["error"]["message"] = "execution reverted";
        js_rsp["error"]["code"] = eth::enum_eth_rpc_execution_reverted;
        js_rsp["error"]["data"] = extra_msg;
        if (!extra_msg.empty() && extra_msg != "0x"){
            auto t = evm_common::xabi_decoder_t::build_from_hex_string(extra_msg);
            auto selector = t.extract<evm_common::xfunction_selector_t>();
            if (selector.method_id == abi_error) {
                js_rsp["error"]["message"] = "execution reverted: " + t.extract<std::string>();
            }
        }       
        break;
    }

    case evm_common::OutOfGas:
        js_rsp["error"]["code"] = eth::enum_eth_rpc_default_error;
        js_rsp["error"]["message"] = "execution out of gas";
        break;

    case evm_common::OutOfOffset:
        js_rsp["error"]["code"] = eth::enum_eth_rpc_default_error;
        js_rsp["error"]["message"] = "execution out of offset";
        break;

    case evm_common::OtherExecuteError:
        js_rsp["error"]["code"] = eth::enum_eth_rpc_default_error;
        js_rsp["error"]["message"] = "execution unknown error";
        break;

    default:
        assert(false);
        break;
    }
}
void xrpc_eth_query_manager::eth_getStorageAt(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 3))
        return;
    std::string account = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(account, js_rsp, 0, eth::enum_rpc_type_address))
        return;
    if (!eth::EthErrorCode::check_eth_address(account, js_rsp))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_unknown))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[2].asString(), js_rsp, 2, eth::enum_rpc_type_block))
        return;

    account = xvaccount_t::to_evm_address(account);
    xdbg("xarc_query_manager::eth_getStorageAt account: %s,%s", account.c_str(), js_req[1].asString().c_str());

    ETH_ADDRESS_CHECK_VALID(account)

    xunitstate_ptr_t account_ptr;
    enum_query_result ret = query_account_by_number(account, js_req[2].asString(), account_ptr);
    if (ret == enum_block_not_found) {
        std::string msg = "header not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    } else if (ret == enum_unit_not_found) {
        js_rsp["result"] = "0x0";
        return;
    }
    std::string index_str = js_req[1].asString().substr(2);
    if (index_str.size() % 2 != 0)
        index_str = std::string("0") + index_str;
    index_str = tcash::HexDecode(index_str);
    std::string value_str = account_ptr->get_storage(index_str);
    xinfo("xrpc_eth_query_manager::eth_getStorageAt, %s,%s,%s, %s", js_req[0].asString().c_str(), js_req[1].asString().c_str(), js_req[2].asString().c_str(),
        tcash::HexEncode(value_str).c_str());
    js_rsp["result"] = std::string("0x") + tcash::HexEncode(value_str);
}
int xrpc_eth_query_manager::parse_tcashics(const Json::Value& tcashics, std::vector<std::set<std::string>>& vtcashics, Json::Value & js_rsp) {
    for (int i = 0; i < (int)tcashics.size(); i++) {
        Json::Value one_tcashic = tcashics[i];
        if (one_tcashic.isString()) {
            if (!eth::EthErrorCode::check_hex(one_tcashic.asString(), js_rsp, 0, eth::enum_rpc_type_tcashic))
                return 1;
            std::set<std::string> s;
            s.insert(one_tcashic.asString());
            vtcashics.push_back(s);
            xdbg("eth_getLogs, tcashics: %s", one_tcashic.asString().c_str());
        } else if (one_tcashic.isArray()) {
            std::set<std::string> s;
            for (int j = 0; j < (int)one_tcashic.size(); j++) {
                if (one_tcashic[j].isString()) {
                    if (!eth::EthErrorCode::check_hex(one_tcashic[j].asString(), js_rsp, 0, eth::enum_rpc_type_tcashic))
                        return 1;
                    s.insert(one_tcashic[j].asString());
                    continue;
                } else if (one_tcashic[j].isNull()) {
                    continue;
                }
                std::string msg = "parse error";
                eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
                return 1;
            }
            vtcashics.push_back(s);
            xdbg("eth_getLogs, tcashics: %d", s.size());
        } else if (one_tcashic.isNull()) {
            std::set<std::string> s;
            vtcashics.push_back(s);
            xdbg("eth_getLogs, tcashics: null");
        } else {
            std::string msg = "parse error";
            eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
            return 1;
        }
    }
    return 0;
}
void xrpc_eth_query_manager::eth_getLogs(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (js_req.size() == 0) {
        std::string msg = std::string("missing value for required argument 0");
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    std::string from_block;
    if (js_req[0].isMember("fromBlock")) {
        from_block = js_req[0]["fromBlock"].asString();
        if (!eth::EthErrorCode::check_hex(from_block, js_rsp, 0, eth::enum_rpc_type_block))
            return;
    }

    std::string to_block;
    if (js_req[0].isMember("toBlock")) {
        to_block = js_req[0]["toBlock"].asString();
        if (!eth::EthErrorCode::check_hex(to_block, js_rsp, 0, eth::enum_rpc_type_block))
            return;
    }

    std::string blockhash;
    if (js_req[0].isMember("blockhash")) {
        blockhash = js_req[0]["blockhash"].asString();
        if (!eth::EthErrorCode::check_hex(blockhash, js_rsp, 0, eth::enum_rpc_type_hash))
            return;
        if (!eth::EthErrorCode::check_hash(blockhash, js_rsp))
            return;
    }

    std::vector<std::set<std::string>> vtcashics;
    if (js_req[0].isMember("tcashics")) {
        Json::Value t = js_req[0]["tcashics"];
        if (!t.isArray()) {
            std::string msg = "parse error";
            eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
            return;
        }
        if (parse_tcashics(t, vtcashics, js_rsp) != 0)
            return;
    }
    std::set<std::string> sAddress;
    if (js_req[0].isMember("address")) {
        Json::Value t = js_req[0]["address"];
        if (t.isString()) {
            if (!eth::EthErrorCode::check_hex(t.asString(), js_rsp, 0, eth::enum_rpc_type_address))
                return;
            if (!eth::EthErrorCode::check_eth_address(t.asString(), js_rsp))
                return;

            sAddress.insert(t.asString());
            xdbg("eth_getLogs, address : %s", t.asString().c_str());
        } else if (t.isArray()) {
            for (int i = 0; i < (int)t.size(); i++) {
                if (!eth::EthErrorCode::check_hex(t[i].asString(), js_rsp, 0, eth::enum_rpc_type_address))
                    return;
                if (!eth::EthErrorCode::check_eth_address(t[i].asString(), js_rsp))
                    return;
                sAddress.insert(t[i].asString());
                xdbg("eth_getLogs, address: %s", t[i].asString().c_str());
            }
        }
    }

    if ((!from_block.empty() || !to_block.empty()) && !blockhash.empty()) {
        std::string msg = "invalid argument 0: cannot specify both BlockHash and FromBlock/ToBlock, choose one or the other";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    uint64_t begin;
    uint64_t end;
    if (!blockhash.empty()) {
        uint256_t hash = tcash::data::hex_to_uint256(blockhash);
        std::string block_hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
        base::xauto_ptr<base::xvblock_t> block = m_block_store->get_block_by_hash(block_hash_str);
        if (block == nullptr) {
            std::string msg = "unknown block";
            eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
            return;
        }
        begin = block->get_height();
        end = block->get_height();
    } else if (from_block.empty() && to_block.empty()){
        js_rsp["result"] = Json::Value::null;
        return;
    } else {
        if (from_block.empty()) {
            std::string addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).to_string();
            base::xvaccount_t _vaddress(addr);
            begin = m_block_store->get_latest_cert_block_height(_vaddress);
        } else
            begin = get_block_height(from_block);
        if (to_block.empty()) {
            std::string addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).to_string();
            base::xvaccount_t _vaddress(addr);
            end = m_block_store->get_latest_cert_block_height(_vaddress);
        } else
            end = get_block_height(to_block);
        if (begin > end) {
            js_rsp["result"] = Json::Value::null;
            return;
        }
        if (end - begin > 1024)
            begin = end - 1024;
    }
    xinfo("xrpc_eth_query_manager::eth_getLogs, %llu, %llu", begin, end);

    get_log(js_rsp, begin, end, vtcashics, sAddress);
    return;
}

void xrpc_eth_query_manager::eth_feeHistory(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 3))
        return;

    // block count:hex string
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_unknown))
        return;

    // Highest block of the requested range: hex string / earliest /  pending /latest
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_block))
        return;

    // A monotonically increasing list of percentile values.
    uint32_t reward_size = 0;
    if (js_req[2].isArray()) {
        auto & reward_list = js_req[2];
        reward_size = reward_list.size();
    } else {
        std::string msg = "cannot unmarshal string into Go value of type []float64";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }

    uint64_t oldest_block_height = 0;
    uint64_t end_block_height = get_block_height(js_req[1].asString());
    uint64_t block_count = std::strtoul(js_req[0].asString().c_str(), NULL, 16);
    if (block_count < 1) {
        std::string msg = "block_count should not be zero";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    if (block_count > 1024) {  // TODO(jimmy) copy from eth, modify to 1024
        xwarn("Sanitizing fee history length requested=%ld, truncated=1024", block_count);
        block_count = 1024;
    }

    if (end_block_height > block_count) {
        oldest_block_height = end_block_height - block_count;
    } else {
        block_count = end_block_height + 1;
    }
    std::stringstream outstr;
    outstr << "0x" << std::hex << oldest_block_height;
    Json::Value js_result;
    js_result["oldestBlock"] = std::string(outstr.str());

    if (reward_size > 0) {
        Json::Value reward_array_json;
        reward_array_json.resize(0);
        for (uint64_t i = 0; i < block_count; i++) {
            Json::Value block_reward_json;
            block_reward_json.resize(0);
            for (uint32_t j = 0; j < reward_size; j++) {
                block_reward_json.append("0x0");
            }
            reward_array_json.append(block_reward_json);
        }
        js_result["reward"] = reward_array_json;
    }

    auto base_price = tcash::gasfee::xgas_estimate::base_price();
    std::string baseprice_hex = tcash::to_hex_prefixed_shrink_0((tcash::evm_common::h256)base_price);
    Json::Value baseFee_array_json;
    baseFee_array_json.resize(0);
    for (uint64_t i = 0; i < (block_count + 1); i++) {
        baseFee_array_json.append(baseprice_hex);
    }
    js_result["baseFeePerGas"] = baseFee_array_json;

    Json::Value gasUsedRatio_array_json;
    gasUsedRatio_array_json.resize(0);
    for (uint64_t i = 0; i < block_count; i++) {
        gasUsedRatio_array_json.append(0.0); // always write to float type data 0.0
    }
    js_result["gasUsedRatio"] = gasUsedRatio_array_json;

    js_rsp["result"] = js_result;
}

bool xrpc_eth_query_manager::check_log_is_match(evm_common::xevm_log_t const& log, const std::vector<std::set<std::string>>& vtcashics, const std::set<std::string>& sAddress) const {
    std::string log_address_hex = log.address.to_hex_string();
    if (!sAddress.empty() && sAddress.find(log_address_hex) == sAddress.end()) {
        xdbg("address not match: %s", log_address_hex.c_str());
        return false;
    }
    if (vtcashics.size() > log.tcashics.size()) {
        xdbg("tcashics size not match %zu,%zu", vtcashics.size(), log.tcashics.size());
        return false;
    }

    uint32_t tcashic_index = 0;
    for (auto & vtcashic : vtcashics) {
        if (vtcashic.empty()) {
            tcashic_index++;
            continue;
        }
        std::string log_tcashic_hex = tcash::to_hex_prefixed(log.tcashics[tcashic_index].asBytes());
        if (vtcashic.find(log_tcashic_hex) == vtcashic.end()) {  // tcashics are order-dependent.
            xdbg("tcashic value not match %d", tcashic_index);
            return false;
        }
        tcashic_index++;
    }
    return true;
}
bool xrpc_eth_query_manager::check_block_log_bloom(xobject_ptr_t<base::xvblock_t>& block, const std::vector<std::set<std::string>>& vtcashics,
    const std::set<std::string>& sAddress) const {
    if (vtcashics.empty() && sAddress.empty())
        return true;

    data::xeth_header_t ethheader;
    std::error_code ec;
    data::xblockextract_t::unpack_ethheader(block.get(), ethheader, ec);
    if (ec) {
        xwarn("check_block_log_bloom fail, block: %llu", block->get_height());
        return false;
    }

    xbytes_t bloom_bytes = ethheader.get_logBloom().get_data();
    evm_common::xbloom9_t block_bloom = evm_common::xbloom9_t::build_from(bloom_bytes, ec);
    if (ec) {
        xwarn("check_block_log_bloom fail, bloom: %d", bloom_bytes.size());
        return false;
    }

    if (!sAddress.empty()) {
        bool address_check = false;
        for (auto address: sAddress) {
            auto address_bytes = tcash::from_hex(address, ec);
            if (ec) {
                xdbg("check_block_log_bloom fail, address: %s", address.c_str());
                return false;
            }
            if (block_bloom.contain(address_bytes)) {
                xdbg("check_block_log_bloom ok, %s", address.c_str());
                address_check = true;
                break;
            }
        }
        if (address_check == false)
            return false;
    }
    if (vtcashics.empty())
        return true;

    for (auto settcashics : vtcashics) {
        if (settcashics.empty())
            continue;
        bool tcashic_check = false;
        for (auto tcashic : settcashics) {  // Each tcashic can also be an array of DATA with “or” options.
            auto tcashic_bytes = tcash::from_hex(tcashic, ec);
            if (ec) {
                xdbg("check_block_log_bloom fail, tcashic: %s", tcashic.c_str());
                return false;
            }

            if (block_bloom.contain(tcashic_bytes)) {
                xdbg("check_block_log_bloom ok, %s", tcashic.c_str());
                tcashic_check = true;
                break;
            }
        }
        if (tcashic_check == false)
            return false;
    }

    return true;
}
int xrpc_eth_query_manager::get_log(Json::Value & js_rsp, const uint64_t begin, const uint64_t end, const std::vector<std::set<std::string>>& vtcashics, const std::set<std::string>& sAddress) {
    auto table_addr = common::xtable_address_t::build_from(common::eth_table_base_address, common::xtable_id_t{0}).vaccount();
    for (uint64_t i = begin; i <= end; i++) {  // traverse blocks
        xobject_ptr_t<base::xvblock_t> block = m_block_store->load_block_object(table_addr, i, base::enum_xvblock_flag_authenticated, false);
        if (block == nullptr) {
            xwarn("xrpc_eth_query_manager::get_log, load_block_object fail:%llu", i);
            continue;
        }

        if (!check_block_log_bloom(block, vtcashics, sAddress)) {
            xdbg("filter_block_log_bloom fail, %llu", i);
            continue;
        } else {
            xdbg("filter_block_log_bloom ok, %llu", i);
        }

        if (false == m_block_store->load_block_input(table_addr, block.get())) {
            xerror("xrpc_eth_query_manager::get_log fail load input %s", block->dump().c_str());
            continue;            
        }

        auto input_actions = data::xblockextract_t::unpack_eth_txactions(block.get());
        xdbg("input_actions size:%d", input_actions.size());
        for (uint64_t txindex = 0; txindex < (uint64_t)input_actions.size(); txindex++) {
            auto & action = input_actions[txindex];

            data::xeth_store_receipt_t evm_tx_receipt;
            auto ret = action.get_evm_transaction_receipt(evm_tx_receipt);
            if (!ret) {
                xerror("xrpc_eth_query_manager::get_log, fail-get evm transaction receipt,i=%llu,txindex=%llu", i, txindex);
                continue;
            }
            if (evm_tx_receipt.get_logs().empty()) {
                continue;
            }

            std::string block_hash = tcash::to_hex_prefixed(block->get_block_hash());
            std::string block_num = xrpc_eth_parser_t::uint64_to_hex_prefixed(block->get_height());
            std::string tx_hash = tcash::to_hex_prefixed(action.get_org_tx_hash());
            std::string tx_index = xrpc_eth_parser_t::uint64_to_hex_prefixed(txindex);
            xlog_location_t loglocation(block_hash, block_num, tx_hash, tx_index);

            // uint32_t index = 0;
            for (uint64_t logindex = 0; logindex < (uint64_t)evm_tx_receipt.get_logs().size(); logindex++) {
                auto & log = evm_tx_receipt.get_logs()[logindex];
                if (false == check_log_is_match(log, vtcashics, sAddress)) {
                    continue;
                }
                loglocation.m_log_index = xrpc_eth_parser_t::uint64_to_hex_prefixed(logindex);

                Json::Value js_log;
                xrpc_eth_parser_t::log_to_json(loglocation, log, js_log);
                js_rsp["result"].append(js_log);
                if (js_rsp["result"].size() >= 1024) {
                    xwarn("xrpc_eth_query_manager::get_log,too many logs: %d,begin=%ld,end=%ld", js_rsp["result"].size(),begin,end);
                    return 0;
                }
            }
        }
    }
    if (js_rsp["result"].empty())
        js_rsp["result"].resize(0);
    return 0;
}

xobject_ptr_t<base::xvblock_t> xrpc_eth_query_manager::query_relay_block_by_height(const std::string& table_height) {
    xdbg("xrpc_eth_query_manager::query_relay_block_by_height: %s, %s", sys_contract_relay_block_addr, table_height.c_str());
    base::xvaccount_t _table_addr(sys_contract_relay_block_addr);

    xobject_ptr_t<base::xvblock_t> _block;
    if (table_height == BlockHeightLatest)
        _block = base::xvchain_t::instance().get_xblockstore()->get_latest_cert_block(_table_addr);
    else if (table_height == BlockHeightEarliest)
        _block = base::xvchain_t::instance().get_xblockstore()->get_genesis_block(_table_addr);
    else if (table_height == BlockHeightPending)
        _block = base::xvchain_t::instance().get_xblockstore()->get_latest_cert_block(_table_addr);
    else {
        uint64_t height = std::strtoul(table_height.c_str(), NULL, 16);
        _block = m_block_store->load_block_object(_table_addr, height, base::enum_xvblock_flag_authenticated, false);
    }
    return _block;
}

int xrpc_eth_query_manager::set_relay_block_result(const xobject_ptr_t<base::xvblock_t>& block, Json::Value & js_rsp, int have_txs, std::string blocklist_type) {
    if (block == nullptr) {
        js_rsp["result"] = Json::Value::null;
        return 1;
    }

    std::error_code ec;
    data::xrelay_block relay_block;
    data::xblockextract_t::unpack_relayblock_from_wrapblock(block.get(), relay_block, ec);
    if (ec) {
        js_rsp["result"] = Json::Value::null;
        xerror("xrpc_eth_query_manager::set_relay_block_result, fail-unpack relayblock.error %s; err msg %s", ec.category().name(), ec.message().c_str());
        return 1;
    }

    xbytes_t header_data = relay_block.streamRLP_header_to_contract();

    Json::Value js_result;
    js_result["header"] = tcash::to_hex_prefixed(header_data);
    relay_block.build_finish();
    js_result["timestamp"] = xrpc::xrpc_eth_parser_t::uint64_to_hex_prefixed(relay_block.get_timestamp());
    js_result["number"] = xrpc::xrpc_eth_parser_t::uint64_to_hex_prefixed(relay_block.get_block_height());
    js_result["hash"] = to_hex_prefixed(relay_block.get_block_hash());
    js_result["parentHash"] = to_hex_prefixed(relay_block.get_header().get_prev_block_hash());
    js_result["receiptsRootHash"] = tcash::to_hex_prefixed(relay_block.get_receipts_root_hash());
    js_result["txsRootHash"] = tcash::to_hex_prefixed(relay_block.get_txs_root_hash());
    js_result["blockRootHash"] = tcash::to_hex_prefixed(relay_block.get_block_merkle_root_hash());
    xbytes_t data = relay_block.encodeBytes();
    js_result["size"] = xrpc::xrpc_eth_parser_t::uint64_to_hex_prefixed(data.size());
    js_result["blockType"] = relay_block.get_block_type_string();
    js_result["chainBits"] = std::string("0x") + relay_block.get_chain_bits().str();

    if (have_txs != 0) {
        const std::vector<xeth_transaction_t> txs = relay_block.get_all_transactions();
        Json::Value js_txs;
        js_txs.resize(0);
        uint64_t index = 0;
        for ( auto &tx: txs) {
            std::string tx_hash = std::string(reinterpret_cast<char*>(tx.get_tx_hash().data()), tx.get_tx_hash().size());
            tx_hash = tcash::to_hex_prefixed(tx_hash);
            if (have_txs == 1) {
                js_txs.append(tx_hash);
                continue;
            }

            std::string block_hash = tcash::to_hex_prefixed(relay_block.get_block_hash());
            std::string block_num = xrpc_eth_parser_t::uint64_to_hex_prefixed(relay_block.get_block_height());
            std::string tx_index = xrpc_eth_parser_t::uint64_to_hex_prefixed(index++);
            xtx_location_t txlocation(block_hash, block_num, tx_hash, tx_index);
            std::error_code ec;
            Json::Value js_tx;

            xrpc_eth_parser_t::transaction_to_json(txlocation, tx, js_tx, ec);
            if (ec) {
                xerror("xrpc_eth_query_manager::set_relay_block_result fail-transaction_to_json.tx hash:%s", tx_hash.c_str());
                continue;
            }
            js_txs.append(js_tx);
        }
        js_result["transactions"] = js_txs;
    }

    Json::Value js_block_list;
    js_block_list.resize(0);
    uint64_t index = 0;
    
    if (blocklist_type == "transaction") {
        std::vector<evm_common::h256> block_hash_vector;
       // data::xrelay_block_store::get_all_leaf_block_hash_list_from_cache(relay_block, block_hash_vector, true);
        enum_block_cache_type block_type = relay_block.check_block_type();
        if (block_type == cache_poly_tx_block) {
            auto &leaf_block_map = relay_block.get_blocks_from_poly();
            for (auto &block_pair:leaf_block_map) {
                Json::Value js_block;
                std::string block_hash = std::string("0x") + block_pair.second.hex();
                js_block["blockIndex"] = xrpc_eth_parser_t::uint64_to_hex_prefixed(index);
                js_block["blockHash"] =  block_hash;
                js_block_list.append(js_block);
                index++;
            }
            
            Json::Value js_block;
            std::string block_hash = std::string("0x") + relay_block.get_block_hash().hex();
            js_block["blockIndex"] = xrpc_eth_parser_t::uint64_to_hex_prefixed(index);
            js_block["blockHash"] =  block_hash;
            js_block_list.append(js_block);
        }
        js_result["blockList"] = js_block_list;
    } else if (blocklist_type == "aggregate") {
        std::map<uint64_t, evm_common::h256> block_hash_map;
        data::xrelay_block_store::get_all_poly_block_hash_list_from_cache(relay_block, block_hash_map);
        for (auto &iter: block_hash_map) {
            Json::Value js_block;
            std::string block_hash = std::string("0x") + iter.second.hex();
            js_block["blockHeight"] = xrpc_eth_parser_t::uint64_to_hex_prefixed(iter.first);
            js_block["blockHash"] =  block_hash;
            js_block_list.append(js_block);
            index++;
        }
        js_result["aggregateList"] = js_block_list;
    }

    js_rsp["result"] = js_result;
    return 0;
}

void xrpc_eth_query_manager::tcashRelay_getBlockByHash(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (js_req.size() == 0) {
        std::string msg = std::string("missing value for required argument 1");
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    if (js_req.size() > 3) {
        std::string msg = std::string("too many arguments, want at most 3");
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    std::string tx_hash = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(tx_hash, js_rsp, 0, eth::enum_rpc_type_hash))
        return;
    if (!eth::EthErrorCode::check_hash(tx_hash, js_rsp))
        return;

    int have_txs = 0;
    std::string blocklist_type;
    if (js_req.size() >= 2){
        if (!js_req[1].isBool()) {
            std::string msg = "parse error";
            eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
            return;
        }

        if (js_req[1].asBool())
            have_txs = 2;
        else
            have_txs = 1;
        
        if (js_req[2].isString()) {
            blocklist_type = js_req[2].asString();
        }
    }

    uint256_t hash = tcash::data::hex_to_uint256(js_req[0].asString());
    std::string block_hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
    xdbg("tcashRelay_getBlockByHash block hash: %s",  tcash::HexEncode(block_hash_str).c_str());

    xobject_ptr_t<base::xvblock_t>  block = m_block_store->get_block_by_hash(block_hash_str);
    set_relay_block_result(block, js_rsp, have_txs, blocklist_type);
}

void xrpc_eth_query_manager::tcashRelay_getBlockByNumber(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (js_req.size() == 0) {
        std::string msg = std::string("missing value for required argument 1");
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    if (js_req.size() > 3) {
        std::string msg = std::string("too many arguments, want at most 3");
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_invalid_params, msg);
        return;
    }
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_block))
        return;

    int have_txs = 0;
    std::string blocklist_type;
    if (js_req.size() >= 2){
        if (!js_req[1].isBool()) {
            std::string msg = "parse error";
            eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_parse_error, msg);
            return;
        }
        if (js_req[1].asBool())
            have_txs = 2;
        else
            have_txs = 1;

        if (js_req[2].isString()) {
            blocklist_type = js_req[2].asString();
        }
    }

    xobject_ptr_t<base::xvblock_t>  block = query_relay_block_by_height(js_req[0].asString());
    set_relay_block_result(block, js_rsp, have_txs, blocklist_type);
}

void xrpc_eth_query_manager::tcashRelay_blockNumber(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 0))
        return;

    base::xvaccount_t _vaddress(sys_contract_relay_block_addr);
    uint64_t height = m_block_store->get_latest_cert_block_height(_vaddress);
    xinfo("xarc_query_manager::tcash_relayBlockNumber: %llu", height);

    std::stringstream outstr;
    outstr << "0x" << std::hex << height;
    js_rsp["result"] = std::string(outstr.str());
}

void xrpc_eth_query_manager::tcashRelay_getTransactionByHash(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 1))
        return;
    std::string tx_hash = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(tx_hash, js_rsp, 0, eth::enum_rpc_type_hash))
        return;
    if (!eth::EthErrorCode::check_hash(tx_hash, js_rsp))
        return;

    uint256_t hash = tcash::data::hex_to_uint256(js_req[0].asString());
    std::string tx_hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
    xdbg("tcash_getRelayTransactionByHash tx hash: %s",  tx_hash.c_str());

    Json::Value js_result;
    xtx_location_t  txlocation{"", ""};
    data::xeth_transaction_t eth_transaction;
    data::xeth_store_receipt_t evm_tx_receipt;
    if(true == xrpc_loader_t::load_relay_tx_indx_detail(tx_hash_str, txlocation, eth_transaction, evm_tx_receipt)) {
        std::error_code ec;
        xrpc_eth_parser_t::transaction_to_json(txlocation, eth_transaction, js_result, ec);
        js_rsp["result"] = js_result;
        xdbg("xrpc_eth_query_manager::tcash_getRelayTransactionByHash ok.tx hash:%s", tx_hash.c_str());
    } else {
        xdbg("xrpc_eth_query_manager::eth_getTransactionByHash fail-transaction_to_json.tx hash:%s", tx_hash.c_str());
        js_rsp["result"] = Json::Value::null;
    }    
    return;
}

void xrpc_eth_query_manager::tcashRelay_getTransactionReceipt(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 1))
        return;
    std::string tx_hash = js_req[0].asString();
    if (!eth::EthErrorCode::check_hex(tx_hash, js_rsp, 0, eth::enum_rpc_type_hash))
        return;
    if (!eth::EthErrorCode::check_hash(tx_hash, js_rsp))
        return;

    std::error_code ec;
    auto tx_hash_bytes = tcash::from_hex(tx_hash, ec);
    if (ec) {
        xdbg("xrpc_eth_query_manager::tcash_getRelayTransactionReceipt from_hex fail: %s",  tx_hash.c_str());
        return;
    }
    std::string raw_tx_hash = tcash::to_string(tx_hash_bytes);
    xdbg("xrpc_eth_query_manager::tcash_getRelayTransactionReceipt tx hash: %s",  tx_hash.c_str());

    Json::Value js_result;
    xtx_location_t  txlocation{"", ""};
    data::xeth_transaction_t eth_transaction;
    data::xeth_store_receipt_t evm_tx_receipt;
    if(true == xrpc_loader_t::load_relay_tx_indx_detail(raw_tx_hash, txlocation, eth_transaction, evm_tx_receipt)) {

        std::error_code ec;
        xrpc_eth_parser_t::receipt_to_json(txlocation, eth_transaction,evm_tx_receipt, js_result, ec);
        if (ec) {
            xerror("xrpc_eth_query_manager::tcash_getRelayTransactionReceipt fail-transaction_to_json.tx hash:%s", tx_hash.c_str());
            js_rsp["result"] = Json::Value::null;
            return;
        }
        js_rsp["result"] = js_result;
        xdbg("xrpc_eth_query_manager::tcash_getRelayTransactionReceipt ok.tx hash:%s", tx_hash.c_str());
    } else {
        xwarn("xrpc_eth_query_manager::tcash_getRelayTransactionReceipt fail-transaction_to_json.tx hash:%s", tx_hash.c_str());
        js_rsp["result"] = Json::Value::null;
    }    
    return;
}

void xrpc_eth_query_manager::tcash_getBalance(Json::Value & js_req, Json::Value & js_rsp, string & strResult, uint32_t & nErrorCode) {
    if (!eth::EthErrorCode::check_req(js_req, js_rsp, 2))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[0].asString(), js_rsp, 0, eth::enum_rpc_type_address))
        return;
    if (!eth::EthErrorCode::check_eth_address(js_req[0].asString(), js_rsp))
        return;
    if (!eth::EthErrorCode::check_hex(js_req[1].asString(), js_rsp, 1, eth::enum_rpc_type_block))
        return;

    std::string account = js_req[0].asString();
    account = xvaccount_t::to_evm_address(account);
    xdbg("xrpc_eth_query_manager::tcash_getBalance account: %s,%s", account.c_str(), js_req[1].asString().c_str());

    ETH_ADDRESS_CHECK_VALID(account)

    data::xunitstate_ptr_t account_ptr;
    enum_query_result ret = query_account_by_number(account, js_req[1].asString(), account_ptr);

    if (ret == enum_block_not_found) {
        std::string msg = "header not found";
        eth::EthErrorCode::deal_error(js_rsp, eth::enum_eth_rpc_default_error, msg);
        return;
    } else if (ret == enum_unit_not_found) {
        js_rsp["result"] = "0x0";
    } else if (ret == enum_success) {
        evm_common::u256 balance = account_ptr->balance();
        js_rsp["result"] = xrpc_eth_parser_t::u256_to_hex_prefixed(balance);
        xdbg("xrpc_eth_query_manager::tcash_getBalance address=%s,balance=%s,%s", account.c_str(), balance.str().c_str(), xrpc_eth_parser_t::u256_to_hex_prefixed(balance).c_str());
    }
}


}  // namespace chain_info
}  // namespace tcash
