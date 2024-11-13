#include "xvledger/xvtxindex.h"
#include "xrpc/xrpc_eth_parser.h"
#include "xbasic/xhex.h"
#include "xcommon/xerror/xerror.h"
#include "xcommon/xeth_address.h"
#include "xcommon/xaddress.h"
#include "xdata/xethheader.h"
#include "xdata/xblockextract.h"
#include "xdata/xethreceipt.h"
#include "xdata/xtx_factory.h"
#include "xdata/xerror/xerror.h"

namespace tcash {

namespace xrpc {

std::string xrpc_eth_parser_t::uint64_to_hex_prefixed(uint64_t value) {
    std::stringstream outstr;
    outstr << "0x" << std::hex << value;
    return outstr.str();
}

std::string xrpc_eth_parser_t::u256_to_hex_prefixed(evm_common::u256 const& value) {
    // TODO(jimmy) optimize
    std::string value_str = toHex((tcash::evm_common::h256)value);

    uint32_t i = 0;
    for (; i < value_str.size() - 1; i++) {
        if (value_str[i] != '0') {
            break;
        }
    }
    return std::string("0x") + value_str.substr(i);
}

void xrpc_eth_parser_t::log_to_json(xlog_location_t const& loglocation, evm_common::xevm_log_t const& log, Json::Value & js_v) {
    js_v["blockNumber"] = loglocation.m_block_number;
    js_v["blockHash"] = loglocation.m_block_hash;
    js_v["transactionHash"] = loglocation.m_tx_hash;
    js_v["transactionIndex"] = loglocation.m_transaction_index;
    js_v["logIndex"] = loglocation.m_log_index;
    js_v["removed"] = false;
    js_v["address"] = log.address.to_hex_string();
    for (auto & tcashic : log.tcashics) {
        js_v["tcashics"].append(tcash::to_hex_prefixed(tcashic.asBytes()));
    }
    js_v["data"] = tcash::to_hex_prefixed(log.data);
}

void xrpc_eth_parser_t::log_to_json_for_tcash_rpc(uint32_t log_idx, evm_common::xevm_log_t const& log, Json::Value & js_v) {
    js_v["logIndex"] = log_idx;
    js_v["removed"] = false;
    js_v["address"] = common::xaccount_address_t::build_from(log.address, base::enum_vaccount_addr_type_secp256k1_evm_user_account).to_string();
    for (auto & tcashic : log.tcashics) {
        js_v["tcashics"].append(tcash::to_hex_prefixed(tcashic.asBytes()));
    }
    js_v["data"] = tcash::to_hex_prefixed(log.data);
}

void xrpc_eth_parser_t::receipt_to_json(const std::string & tx_hash, xtxindex_detail_ptr_t const& sendindex, Json::Value & js_v, std::error_code & ec) {
    std::string block_hash = tcash::to_hex_prefixed(sendindex->get_block_hash());
    std::string block_num = uint64_to_hex_prefixed(sendindex->get_txindex()->get_block_height());
    std::string tx_idx = uint64_to_hex_prefixed((uint64_t)sendindex->get_transaction_index());
    xtx_location_t txlocation(block_hash,block_num,tx_hash,tx_idx);

    data::xeth_store_receipt_t evm_tx_receipt;
    auto ret = sendindex->get_txaction().get_evm_transaction_receipt(evm_tx_receipt);
    if (!ret) {
        ec = common::error::xerrc_t::invalid_block;
        xerror("xrpc_eth_parser_t::receipt_to_json fail-get_evm_receipt");
        return;
    }

    data::xeth_transaction_t ethtx = sendindex->get_raw_tx()->to_eth_tx(ec);
    if (ec) {
        xerror("xrpc_eth_parser_t::receipt_to_json fail-to eth tx");
        return;
    }
    receipt_to_json(txlocation, ethtx, evm_tx_receipt, js_v, ec);
}

void xrpc_eth_parser_t::receipt_to_json_for_tcash_rpc(const std::string & tx_hash, xtxindex_detail_ptr_t const& sendindex, Json::Value & js_v, std::error_code & ec) {
    data::xeth_store_receipt_t evm_tx_receipt;
    auto ret = sendindex->get_txaction().get_evm_transaction_receipt(evm_tx_receipt);
    if (!ret) {
        ec = common::error::xerrc_t::invalid_block;
        xerror("receipt_to_json_for_tcash_rpc::receipt_to_json fail-get_evm_receipt");
        return;
    }

    data::xeth_transaction_t ethtx = sendindex->get_raw_tx()->to_eth_tx(ec);
    if (ec) {
        xerror("xrpc_eth_parser_t::receipt_to_json fail-to eth tx");
        return;
    }
    receipt_to_json_for_tcash_rpc(tcash::to_hex_prefixed(sendindex->get_block_hash()), (uint32_t)sendindex->get_transaction_index(), ethtx, evm_tx_receipt, js_v, ec);
}

void xrpc_eth_parser_t::transaction_to_json(xtx_location_t const& txlocation, data::xtransaction_ptr_t const& rawtx, Json::Value & js_v, std::error_code & ec) {
    // txlocation
    js_v["blockHash"] = txlocation.m_block_hash;
    js_v["blockNumber"] = txlocation.m_block_number;
    js_v["hash"] = txlocation.m_tx_hash;
    js_v["transactionIndex"] = txlocation.m_transaction_index;

    data::xeth_transaction_t ethtx = rawtx->to_eth_tx(ec);
    if (ec) {
        return;
    }
    transaction_to_json(txlocation, ethtx, js_v, ec);
}
void xrpc_eth_parser_t::blockheader_to_json(base::xvblock_t* _block, Json::Value & js_v, std::error_code & ec) {
    data::xeth_header_t ethheader;
    data::xblockextract_t::unpack_ethheader(_block, ethheader, ec);
    if (ec) {
        return;
    }

    // not need implement for tcash
    js_v["difficulty"] = "0x0";
    js_v["mixHash"] = std::string("0x") + std::string(64, '0');
    js_v["nonce"] = std::string("0x") + std::string(16, '0');
    js_v["totalDifficulty"] = "0x0";
    js_v["uncles"].resize(0);
    js_v["sha3Uncles"] = std::string("0x") + std::string(64, '0');

    js_v["size"] = uint64_to_hex_prefixed(_block->get_block_size());
    js_v["gasLimit"] = uint64_to_hex_prefixed(ethheader.get_gaslimit());
    js_v["gasUsed"] = uint64_to_hex_prefixed(ethheader.get_gasused());
    js_v["hash"] = tcash::to_hex_prefixed(_block->get_block_hash());
    js_v["logsBloom"] = tcash::to_hex_prefixed(ethheader.get_logBloom().get_data());
    js_v["number"] = uint64_to_hex_prefixed(_block->get_height());
    js_v["parentHash"] = tcash::to_hex_prefixed(_block->get_last_block_hash());
    js_v["receiptsRoot"] = tcash::to_hex_prefixed(ethheader.get_receipts_root().asBytes());
    js_v["transactionsRoot"] = tcash::to_hex_prefixed(ethheader.get_transactions_root().asBytes());
    js_v["stateRoot"] = tcash::to_hex_prefixed(ethheader.get_state_root().asBytes());
    js_v["timestamp"] = uint64_to_hex_prefixed(_block->get_timestamp());
    js_v["baseFeePerGas"] = u256_to_hex_prefixed(ethheader.get_baseprice());
    js_v["miner"] = ethheader.get_coinbase().to_hex_string();
    js_v["extraData"] = tcash::to_hex_prefixed(ethheader.get_extra_data());
    js_v["transactions"].resize(0);
}

data::xtransaction_ptr_t xrpc_eth_parser_t::json_to_ethtx(Json::Value const& request, data::eth_error& ec) {
    if (request["params"].empty()) {
        ec = data::eth_error(data::error::xenum_errc::eth_invalid_params, "missing value for required argument 0");
        return nullptr;
    }
    if (!request["params"].isArray()) {
        ec = data::eth_error(data::error::xenum_errc::eth_invalid_params, "non-array args");
        return nullptr;
    }
    if (!request["params"][0].isString()) {
        ec = data::eth_error(data::error::xenum_errc::eth_invalid_params, "invalid argument 0: json: cannot unmarshal non-string into Go value of type hexutil.Bytes");
        return nullptr;
    }
    std::string strParams = request["params"][0].asString();
    if (strParams.size() % 2) {
        ec = data::eth_error(data::error::xenum_errc::eth_invalid_params, "invalid argument 0: json: cannot unmarshal hex string of odd length into Go value of type hexutil.Bytes");
        return nullptr;
    }

    if (strParams.size() <= 10) {
        ec = data::eth_error(data::error::xenum_errc::eth_invalid_params, "rlp: value size exceeds available input length");
        return nullptr;
    }
    if (strParams[0] != '0' || strParams[1] != 'x') {
        ec = data::eth_error(data::error::xenum_errc::eth_invalid_params, "invalid argument 0: json: cannot unmarshal hex string without 0x prefix into Go value of type hexutil.Bytes");
        return nullptr;
    }
    data::xeth_transaction_t ethtx = data::xeth_transaction_t::build_from(strParams, ec);
    if (ec.error_code) {
        return nullptr;
    }

    data::xtransaction_ptr_t tx = data::xtx_factory::create_v3_tx(ethtx);
    xinfo("xrpc_eth_parser_t::json_to_ethtx succ.tx=%s", tx->dump().c_str());
    return tx;
}



void xrpc_eth_parser_t::transaction_to_json(xtx_location_t const& txlocation, data::xeth_transaction_t const& ethtx, Json::Value & js_v, std::error_code & ec) {
    // txlocation
    js_v["blockHash"] = txlocation.m_block_hash;
    js_v["blockNumber"] = txlocation.m_block_number;
    js_v["hash"] = txlocation.m_tx_hash;
    js_v["transactionIndex"] = txlocation.m_transaction_index;

    js_v["from"] = ethtx.get_from().to_hex_string();
    js_v["gas"] = u256_to_hex_prefixed(ethtx.get_gas());
    js_v["gasPrice"] = u256_to_hex_prefixed(ethtx.get_max_fee_per_gas());
    js_v["maxPriorityFeePerGas"] = u256_to_hex_prefixed(ethtx.get_max_priority_fee_per_gas());
    js_v["maxFeePerGas"] = u256_to_hex_prefixed(ethtx.get_max_fee_per_gas());
    js_v["input"] = tcash::to_hex_prefixed(ethtx.get_data());
    js_v["nonce"] = u256_to_hex_prefixed(ethtx.get_nonce());
    if (ethtx.get_ethtx_type() == data::enum_ethtx_type::enum_ethtx_type_message_call) {
        js_v["to"] = ethtx.get_to().to_hex_string();
    } else {
        js_v["to"] = Json::Value::null;
    }
    js_v["type"] = uint64_to_hex_prefixed((uint64_t)ethtx.get_tx_version());
    js_v["value"] = u256_to_hex_prefixed(ethtx.get_value());
    js_v["v"] = u256_to_hex_prefixed(ethtx.get_signV());
    js_v["r"] = tcash::to_hex_prefixed(ethtx.get_signR().asBytes());
    js_v["s"] = tcash::to_hex_prefixed(ethtx.get_signS().asBytes());
}


void xrpc_eth_parser_t::receipt_to_json(xtx_location_t const& txlocation,  data::xeth_transaction_t const& ethtx,
                                        data::xeth_store_receipt_t const &evm_tx_receipt,Json::Value & js_v, std::error_code & ec) {

    js_v["blockNumber"] = txlocation.m_block_number;
    js_v["blockHash"] = txlocation.m_block_hash;
    js_v["transactionHash"] = txlocation.m_tx_hash;
    js_v["transactionIndex"] = txlocation.m_transaction_index;

    js_v["status"] = (evm_tx_receipt.get_tx_status() == data::ethreceipt_status_successful) ?  "0x1" : "0x0";
    js_v["type"] = uint64_to_hex_prefixed((uint64_t)ethtx.get_tx_version());
    js_v["gasUsed"] = uint64_to_hex_prefixed(evm_tx_receipt.get_gas_used());
    js_v["cumulativeGasUsed"] = uint64_to_hex_prefixed(evm_tx_receipt.get_cumulative_gas_used());
    js_v["effectiveGasPrice"] = xrpc_eth_parser_t::u256_to_hex_prefixed(evm_tx_receipt.get_gas_price());


    js_v["from"] =  ethtx.get_from().to_hex_string();
    if (ethtx.get_ethtx_type() == data::enum_ethtx_type::enum_ethtx_type_message_call) {
        js_v["to"] = ethtx.get_to().to_hex_string();
        if (!ethtx.get_data().empty()) {  // call contract
            js_v["contractAddress"] = ethtx.get_to().to_hex_string();
        } else { // transfer
            js_v["contractAddress"] = Json::Value::null;
        }
    } else {
        js_v["to"] = Json::Value::null;
        if (evm_tx_receipt.get_tx_status() == data::ethreceipt_status_successful) {
            js_v["contractAddress"] = evm_tx_receipt.get_contract_address().to_hex_string();
        } else {
            js_v["contractAddress"] = Json::Value::null;
        }
    }

    evm_common::xbloom9_t logs_bloom = evm_tx_receipt.bloom();
    js_v["logsBloom"] = tcash::to_hex_prefixed(logs_bloom.get_data());

    if (!evm_tx_receipt.get_logs().empty()) {
        xlog_location_t loglocation(txlocation.m_block_hash, txlocation.m_block_number, txlocation.m_tx_hash, txlocation.m_transaction_index);
        uint64_t index = 0;
        for (auto & log : evm_tx_receipt.get_logs()) {
            loglocation.m_log_index = uint64_to_hex_prefixed(index);
            index++;
            Json::Value js_log;
            log_to_json(loglocation, log, js_log);
            js_v["logs"].append(js_log);
        }
    } else {
        js_v["logs"].resize(0);
    }
}

void xrpc_eth_parser_t::receipt_to_json_for_tcash_rpc(std::string const & blockhash, uint32_t tx_idx,  data::xeth_transaction_t const& ethtx,
                                        data::xeth_store_receipt_t const &evm_tx_receipt,Json::Value & js_v, std::error_code & ec) {
    // fields from eth_getTransactionReceipt
    js_v["blockHash"] = blockhash;
    js_v["transactionIndex"] = tx_idx;
    js_v["status"] = (evm_tx_receipt.get_tx_status() == data::ethreceipt_status_successful) ? 1 : 0;
    js_v["type"] = (uint32_t)ethtx.get_tx_version();
    js_v["gasUsed"] = std::to_string(evm_tx_receipt.get_gas_used());
    js_v["cumulativeGasUsed"] = std::to_string(evm_tx_receipt.get_cumulative_gas_used());
    js_v["effectiveGasPrice"] = evm_tx_receipt.get_gas_price().str();

    if (ethtx.get_ethtx_type() == data::enum_ethtx_type::enum_ethtx_type_message_call) {
        if (!ethtx.get_data().empty()) {
            js_v["contractAddress"] = common::xaccount_address_t::build_from(ethtx.get_to(), base::enum_vaccount_addr_type_secp256k1_evm_user_account).to_string();
        } else {
            js_v["contractAddress"] = Json::Value::null;
        }
    } else {
        if (evm_tx_receipt.get_tx_status() == data::ethreceipt_status_successful) {
            js_v["contractAddress"] = common::xaccount_address_t::build_from(evm_tx_receipt.get_contract_address(),
                                                                            base::enum_vaccount_addr_type_secp256k1_evm_user_account).to_string();            
        } else {
            js_v["contractAddress"] = Json::Value::null;
        }
    }

    evm_common::xbloom9_t logs_bloom = evm_tx_receipt.bloom();
    js_v["logsBloom"] = tcash::to_hex_prefixed(logs_bloom.get_data());

    if (!evm_tx_receipt.get_logs().empty()) {
        uint32_t index = 0;
        for (auto & log : evm_tx_receipt.get_logs()) {
            index++;
            Json::Value js_log;
            log_to_json_for_tcash_rpc(index, log, js_log);
            js_v["logs"].append(js_log);
        }
    } else {
        js_v["logs"].resize(0);
    }

    // fields from eth_getTransactionByHash
    js_v["input"] = tcash::to_hex_prefixed(ethtx.get_data());
    js_v["gas"] = ethtx.get_gas().str();
    js_v["gasPrice"] = ethtx.get_max_fee_per_gas().str();
    js_v["maxPriorityFeePerGas"] = ethtx.get_max_priority_fee_per_gas().str();
    js_v["maxFeePerGas"] = ethtx.get_max_fee_per_gas().str();
    js_v["value"] = ethtx.get_value().str();
}

}  // namespace chain_info
}  // namespace tcash
