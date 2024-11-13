// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include "xdata/xtransaction.h"
#include "xdata/xblockaction.h"
#include <map>
#include <mutex>

namespace tcash { namespace data {

struct xtransaction_cache_data_t {
    xtransaction_ptr_t tran;
    //Json::Value jv;
    std::map<int, Json::Value> jv;
    data::xlightunit_action_ptr_t   recv_txinfo{nullptr};
};
class xtransaction_cache_t{
public:
    //static xtransaction_cache_t & instance();
    xtransaction_cache_t() {
    }
    ~xtransaction_cache_t() {}
    bool tx_add(const std::string& tx_hash, const xtransaction_ptr_t tx);
    int tx_find(const std::string& tx_hash);
    int tx_get_json(const std::string& tx_hash, const int index, Json::Value & jv);
    int tx_set_json(const std::string& tx_hash, const int index, const Json::Value & jv);
    int tx_set_recv_txinfo(const std::string& tx_hash, const data::xlightunit_action_ptr_t tx_info);
    bool tx_get(const std::string& tx_hash, xtransaction_cache_data_t& cache_data);
    int tx_erase(const std::string& tx_hash);
    int tx_clean();
    int tx_clear();
private:
    std::map<std::string, xtransaction_cache_data_t> m_trans;
    std::mutex  m_mutex;
};

}
}
