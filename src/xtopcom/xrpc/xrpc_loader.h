#pragma once

#include <json/json.h>
#include "xvledger/xvtxindex.h"
#include "xvledger/xvaction.h"
#include "xdata/xlightunit_info.h"
#include "xdata/xtransaction.h"
#include "xdata/xethreceipt.h"

namespace tcash {

namespace xrpc {

class xtxindex_detail_t {
public:
    xtxindex_detail_t(const base::xvtxindex_ptr & txindex, std::string const & blockhash, const base::xvaction_t & txaction, uint64_t transaction_index, uint64_t blocktimestamp);
    ~xtxindex_detail_t() {}

    void            set_raw_tx(base::xdataunit_t* tx);
    const base::xvtxindex_ptr &         get_txindex() const {return m_txindex;}
    const std::string &                 get_block_hash() const {return m_block_hash;}
    const data::xlightunit_action_t &   get_txaction() const {return m_txaction;}
    const data::xtransaction_ptr_t &    get_raw_tx() const {return m_raw_tx;}
    uint64_t                            get_transaction_index() const {return m_transaction_index;}
    uint64_t                            get_blocktimestamp() const {return m_blocktimestamp;}

private:
    base::xvtxindex_ptr         m_txindex{nullptr};
    std::string                 m_block_hash;
    data::xlightunit_action_t   m_txaction;
    data::xtransaction_ptr_t    m_raw_tx{nullptr};
    uint64_t                    m_transaction_index{0};
    uint64_t                    m_blocktimestamp{0};
};
using xtxindex_detail_ptr_t = std::shared_ptr<xtxindex_detail_t>;

struct xtx_location_t;
class xrpc_loader_t {
 public:
    static  xtxindex_detail_ptr_t   load_tx_indx_detail(const std::string & raw_tx_hash,base::enum_transaction_subtype type);
 public:  // json transfer
    static  Json::Value            parse_send_tx(const xtxindex_detail_ptr_t & txindex_detail);
    static  Json::Value            load_and_parse_recv_tx(const std::string & raw_tx_hash, const xtxindex_detail_ptr_t & sendindex, xtxindex_detail_ptr_t & recvindex, data::enum_xunit_tx_exec_status & recvtx_status);
    static  Json::Value            load_and_parse_confirm_tx(const std::string & raw_tx_hash, const xtxindex_detail_ptr_t & sendindex,data::enum_xunit_tx_exec_status recvtx_status);
    static  void                   parse_logs(const xtxindex_detail_ptr_t & index, Json::Value & jv);
 private:
    static  Json::Value            parse_recv_tx(const xtxindex_detail_ptr_t & sendindex, const xtxindex_detail_ptr_t & recvindex);
    static  Json::Value            parse_confirm_tx(const xtxindex_detail_ptr_t & sendindex, data::enum_xunit_tx_exec_status recvtx_status, const xtxindex_detail_ptr_t & confirmindex);
    static  void                    parse_common_info(const xtxindex_detail_ptr_t & txindex, Json::Value & jv);

 public: // load ethdata
    static  xtxindex_detail_ptr_t   load_ethtx_indx_detail(const std::string & raw_tx_hash);
 
 public: //load relay block data
    static  bool                    load_relay_tx_indx_detail(const std::string & raw_tx_hash, xtx_location_t &txlocation, 
                                                              data::xeth_transaction_t &eth_transaction, 
                                                              data::xeth_store_receipt_t &evm_tx_receipt);
};

}  // namespace chain_info
}  // namespace tcash
