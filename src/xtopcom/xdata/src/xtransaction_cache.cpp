#include "xdata/xtransaction_cache.h"
#include "xpbase/base/tcash_utils.h"

namespace tcash { namespace data {

bool xtransaction_cache_t::tx_add(const std::string& tx_hash, const xtransaction_ptr_t tx) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto iter = m_trans.find(tx_hash);
    if (iter != m_trans.end()) {
        XMETRICS_GAUGE(metrics::txstore_request_origin_tx, 0);
        return false;
    }

    xtransaction_cache_data_t cache_data;
    cache_data.tran = tx;
    m_trans[tx_hash] = cache_data;
    xdbg("add cache: %s, size:%d", tcash::HexEncode(tx_hash).c_str(), sizeof(tx_hash) + sizeof(cache_data));
    XMETRICS_GAUGE(metrics::txstore_request_origin_tx, 1);
    XMETRICS_GAUGE(metrics::txstore_cache_origin_tx, 1);
    return true;
}
int xtransaction_cache_t::tx_find(const std::string& tx_hash) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trans.find(tx_hash) == m_trans.end())
        return 0;
    return 1;
}
int xtransaction_cache_t::tx_get_json(const std::string& tx_hash, const int index, Json::Value & jv) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trans.find(tx_hash) == m_trans.end())
        return 0;
    jv = m_trans[tx_hash].jv[index];
    return 1;
}
bool xtransaction_cache_t::tx_get(const std::string& tx_hash, xtransaction_cache_data_t& cache_data){
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trans.find(tx_hash) == m_trans.end())
        return false;
    cache_data = m_trans[tx_hash];
    return true;
}
int xtransaction_cache_t::tx_set_json(const std::string& tx_hash, const int index, const Json::Value & jv) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trans.find(tx_hash) == m_trans.end())
        return 1;
    m_trans[tx_hash].jv[index] = jv;
    xdbg("add cache json: %d, %s", index, tcash::HexEncode(tx_hash).c_str());
    return 0;
}
int xtransaction_cache_t::tx_set_recv_txinfo(const std::string& tx_hash, const data::xlightunit_action_ptr_t tx_info){
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trans.find(tx_hash) == m_trans.end())
        return 1;
    m_trans[tx_hash].recv_txinfo = tx_info;
    xdbg("add cache recv_txinfo: %s", tcash::HexEncode(tx_hash).c_str());
    return 0;
}
int xtransaction_cache_t::tx_erase(const std::string& tx_hash) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, xtransaction_cache_data_t>::iterator it = m_trans.find(tx_hash);
    if (it == m_trans.end())
        return 1;
    m_trans.erase(it);
    xdbg("erase cache: %s", tcash::HexEncode(tx_hash).c_str());
    XMETRICS_GAUGE(metrics::txstore_cache_origin_tx, -1);
    return 0;
}
int xtransaction_cache_t::tx_clean() {
    struct timeval val;
    base::xtime_utl::gettimeofday(&val);
    std::lock_guard<std::mutex> lock(m_mutex);
    for (std::map<std::string, xtransaction_cache_data_t>::iterator it= m_trans.begin(); it != m_trans.end(); ) {
        if (it->second.tran->get_fire_timestamp() + it->second.tran->get_expire_duration() < (uint64_t)val.tv_sec) {
            xdbg("erase tx: %lld,%d,%lld", it->second.tran->get_fire_timestamp() , it->second.tran->get_expire_duration() , (uint64_t)val.tv_sec);
            m_trans.erase(it++);
            XMETRICS_GAUGE(metrics::txstore_cache_origin_tx, -1);
            continue;
        }
        xdbg("not erase tx: %lld,%d,%lld", it->second.tran->get_fire_timestamp() , it->second.tran->get_expire_duration() , (uint64_t)val.tv_sec);
        ++it;
    }
    return 0;
}
int xtransaction_cache_t::tx_clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_trans.clear();
    xinfo("cleat tx cache all.");
    return 0;
}
}
}
