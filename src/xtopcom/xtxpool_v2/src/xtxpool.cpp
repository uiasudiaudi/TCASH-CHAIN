// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xtxpool_v2/xtxpool.h"

#include "xdata/xblocktool.h"
#include "xdata/xnative_contract_address.h"
#include "xpreprocess/xpreprocess.h"
#include "xtxpool_v2/xtxpool_error.h"
#include "xtxpool_v2/xtxpool_log.h"
#include "xtxpool_v2/xtxpool_para.h"
#include "xvledger/xvledger.h"
#include "xbase/xutl.h"
#include "xbasic/xsys_utl.h"

namespace tcash {
namespace xtxpool_v2 {

using data::xcons_transaction_ptr_t;

xtxpool_t::xtxpool_t(const std::shared_ptr<xtxpool_resources_face> & para) : m_para(para) {
    std::map<base::enum_xchain_zone_index, uint16_t> const & all_table_indexs = base::xvledger_config_t::get_all_consensus_zone_subaddr_paris();
    for (auto & table_index : all_table_indexs) {
        for (uint16_t i = 0; i < table_index.second; i++) {
            base::xtable_index_t tableindex(table_index.first, i);
            m_all_table_sids.insert(tableindex.to_table_shortid());
        }
        m_tables_mgr.add_tables(table_index.first, table_index.second);
    }

    uint64_t system_memory = xsys_utl_t::get_total_memory();
    if (system_memory < 2.5*1024*1024*1024L) {// less than 2.5G is a validator node
        m_cache_limit.m_total_send_tx_max_num = total_send_tx_queue_size_max_validator;
        m_cache_limit.m_total_recv_tx_max_num = total_recv_tx_queue_size_max_validator;
        m_cache_limit.m_total_confirm_tx_max_num = total_confirm_tx_queue_size_max_validator;
    } else {
        m_cache_limit.m_total_send_tx_max_num = total_send_tx_queue_size_max_auditor;
        m_cache_limit.m_total_recv_tx_max_num = total_recv_tx_queue_size_max_auditor;
        m_cache_limit.m_total_confirm_tx_max_num = total_confirm_tx_queue_size_max_auditor;
    }
    xinfo("xtxpool_t::xtxpool_t momory=%ld",system_memory);
}

bool table_zone_subaddr_check(uint8_t zone, uint16_t subaddr) {
    std::map<base::enum_xchain_zone_index, uint16_t> const & all_table_indexs = base::xvledger_config_t::get_all_consensus_zone_subaddr_paris();
    auto iter = all_table_indexs.find((base::enum_xchain_zone_index)zone);
    if (iter != all_table_indexs.end()) {
        if (subaddr < iter->second) {
            return true;
        }
    }
    xwarn("table_zone_subaddr_check zone:%d or subaddr:%d invalidate", zone, subaddr);
    return false;
}

int32_t xtxpool_t::push_send_tx(const std::shared_ptr<xtx_entry> & tx) {
    // TODO(jimmy) not limit evm table tx now
    if (m_statistic.m_push_tx_send_cur_num >= m_cache_limit.m_total_send_tx_max_num
        && tx->get_tx()->get_self_table_index().get_zone_index() != base::enum_chain_zone_evm_index) {
        xwarn("xtxpool_t::push_send_tx fail-reach send limit.tx=%s,cur=%d",tx->get_tx()->dump().c_str(),m_statistic.m_push_tx_send_cur_num.load());
        return xtxpool_error_pending_reached_upper_limit;
    }

    auto table = get_txpool_table_by_addr(tx);
    if (table == nullptr) {
        return xtxpool_error_account_not_in_charge;
    }

    if (nullptr == table->query_tx(tx->get_tx()->get_tx_hash())) {
        auto f = [tx, table]() {
            assert(tx != nullptr && table != nullptr);
            return table->push_send_tx(tx) == xsuccess;
        };
        data::xmessage_t msg{common::xaccount_address_t{tx->get_tx()->get_source_addr()}, tx->get_tx(), f};
        if (data::xpreprocess::instance().send(msg)) {
            return xsuccess;
        }
    }

    auto ret = table->push_send_tx(tx);
    return ret;
}

int32_t xtxpool_t::push_receipt(const std::shared_ptr<xtx_entry> & tx, bool is_self_send, bool is_pulled) {
    if (tx->get_tx()->is_recv_tx() && m_statistic.m_push_tx_recv_cur_num >= m_cache_limit.m_total_recv_tx_max_num) {
        xwarn("xtxpool_t::push_receipt fail-reach recv limit.tx=%s,cur=%d",tx->get_tx()->dump().c_str(),m_statistic.m_push_tx_recv_cur_num.load());
        return xtxpool_error_pending_reached_upper_limit;
    }
    if (tx->get_tx()->is_confirm_tx() && m_statistic.m_push_tx_confirm_cur_num >= m_cache_limit.m_total_confirm_tx_max_num) {
        xwarn("xtxpool_t::push_receipt fail-reach confirm limit.tx=%s,cur=%d",tx->get_tx()->dump().c_str(),m_statistic.m_push_tx_confirm_cur_num.load());
        return xtxpool_error_pending_reached_upper_limit;
    }

    auto table = get_txpool_table_by_addr(tx);
    if (table == nullptr) {
        return xtxpool_error_account_not_in_charge;
    }
    auto ret = table->push_receipt(tx, is_self_send);

    if (ret == xsuccess) {
        m_statistic.update_receipt_recv_num(tx->get_tx(), is_pulled);
    }
    return ret;
}

void xtxpool_t::print_statistic_values() const {
    m_statistic.print();

    uint32_t sender_cache_size = 0;
    uint32_t receiver_cache_size = 0;
    uint32_t height_record_size = 0;
    uint32_t unconfirm_raw_txs_size = 0;
    uint32_t table_sender_cache_size = 0;
    uint32_t table_receiver_cache_size = 0;
    uint32_t table_height_record_size = 0;
    uint32_t table_unconfirm_raw_txs_size = 0;

    std::map<base::enum_xchain_zone_index, uint16_t> const & all_table_indexs = base::xvledger_config_t::get_all_consensus_zone_subaddr_paris();
    for (auto & table_index : all_table_indexs) {
        for (uint16_t i = 0; i < table_index.second; i++) {
            auto table = get_txpool_table(table_index.first, i);
            if (table != nullptr) {
                table->unconfirm_cache_status(table_sender_cache_size, table_receiver_cache_size, table_height_record_size, table_unconfirm_raw_txs_size);
                xinfo("xtxpool_t::print_statistic_values table:%d,cache size:%u:%u:%u:%u",
                      table->table_sid(),
                      table_sender_cache_size,
                      table_receiver_cache_size,
                      table_height_record_size,
                      table_unconfirm_raw_txs_size);
                sender_cache_size += table_sender_cache_size;
                receiver_cache_size += table_receiver_cache_size;
                height_record_size += table_height_record_size;
                unconfirm_raw_txs_size += table_unconfirm_raw_txs_size;
            }
        }
    }

    XMETRICS_GAUGE_SET_VALUE(metrics::txpool_sender_unconfirm_cache, sender_cache_size);
    XMETRICS_GAUGE_SET_VALUE(metrics::txpool_receiver_unconfirm_cache, receiver_cache_size);
    XMETRICS_GAUGE_SET_VALUE(metrics::txpool_height_record_cache, height_record_size);
    XMETRICS_GAUGE_SET_VALUE(metrics::txpool_table_unconfirm_raw_txs, height_record_size);
}

const xcons_transaction_ptr_t xtxpool_t::pop_tx(const tx_info_t & txinfo) {
    auto table = get_txpool_table_by_addr(txinfo.get_addr());
    if (table == nullptr) {
        return nullptr;
    }
    return table->pop_tx(txinfo, true);
}

xpack_resource xtxpool_t::get_pack_resource(const xtxs_pack_para_t & pack_para) {
    auto table = get_txpool_table_by_addr(pack_para.get_table_addr());
    if (table == nullptr) {
        return {};
    }
    return table->get_pack_resource(pack_para);
}

data::xcons_transaction_ptr_t xtxpool_t::query_tx(const std::string & account_addr, const uint256_t & hash) const {
    auto table = get_txpool_table_by_addr(account_addr);
    if (table == nullptr) {
        xtxpool_warn("xtxpool_t::query_tx table not found, account:%s", account_addr.c_str());
        return nullptr;
    }
    return table->query_tx(hash);
}

data::xcons_transaction_ptr_t xtxpool_t::query_tx(const std::string & account_addr, const std::string & hash_hex) const {
    auto table = get_txpool_table_by_addr(account_addr);
    if (table == nullptr) {
        xtxpool_warn("xtxpool_t::query_tx table not found, account:%s", account_addr.c_str());
        return nullptr;
    }
    return table->query_tx(hash_hex);
}

void xtxpool_t::updata_latest_nonce(const std::string & account_addr, uint64_t latest_nonce) {
    auto table = get_txpool_table_by_addr(account_addr);
    if (table == nullptr) {
        return;
    }
    return table->updata_latest_nonce(account_addr, latest_nonce);
}

void xtxpool_t::subscribe_tables(uint8_t zone, uint16_t front_table_id, uint16_t back_table_id, common::xnode_type_t node_type) {
    xtxpool_info("xtxpool_t::subscribe_tables zone:%d,front_table_id:%d,back_table_id:%d,node_type:%d", zone, front_table_id, back_table_id, node_type);
    if (front_table_id > back_table_id) {
        xerror("xtxpool_t::subscribe_tables table id invalidate front_table_id:%d back_table_id%d,node_type:%d", front_table_id, back_table_id, node_type);
        return;
    }
    if (!table_zone_subaddr_check(zone, back_table_id)) {
        return;
    }

    std::lock_guard<std::mutex> lck(m_mutex[zone]);
    for (uint32_t i = 0; i < m_roles[zone].size(); i++) {
        assert(m_roles[zone][i] != nullptr);
        if (m_roles[zone][i]->is_ids_match(zone, front_table_id, back_table_id, node_type)) {
            m_roles[zone][i]->add_sub_count();
            return;
        }
    }
    auto role = std::make_shared<xtxpool_role_info_t>(zone, front_table_id, back_table_id, node_type);
    m_roles[zone].push_back(role);
    role->add_sub_count();

    xtxpool_info("xtxpool_t::subscribe_tables sub tables:zone:%d,front_table_id:%d,back_table_id:%d,node_type:%d", zone, front_table_id, back_table_id, node_type);

    uint32_t add_table_num = 0;
    for (uint16_t i = front_table_id; i <= back_table_id; i++) {
        bool is_add_new = m_tables_mgr.subscribe_table(zone, i, m_para.get(), role.get(), &m_statistic, &m_all_table_sids);
        if (is_add_new) {
            add_table_num++;
        }
    }
    if (add_table_num > 0) {
        m_statistic.inc_table_num(add_table_num);
        {
            std::lock_guard<std::mutex> lck(m_peer_table_height_cache_mutex);
            m_peer_table_height_cache.clear();
        }
    }
}

void xtxpool_t::unsubscribe_tables(uint8_t zone, uint16_t front_table_id, uint16_t back_table_id, common::xnode_type_t node_type) {
    xtxpool_info("xtxpool_t::unsubscribe_tables zone:%d,front_table_id:%d,back_table_id:%d,node_type:%d", zone, front_table_id, back_table_id, node_type);
    if (front_table_id > back_table_id) {
        xerror("xtxpool_t::unsubscribe_tables table id invalidate front_table_id:%d back_table_id%d,node_type:%d", front_table_id, back_table_id, node_type);
        return;
    }
    if (!table_zone_subaddr_check(zone, back_table_id)) {
        return;
    }
    std::lock_guard<std::mutex> lck(m_mutex[zone]);
    uint32_t remove_table_num = 0;
    for (auto it = m_roles[zone].begin(); it != m_roles[zone].end(); it++) {
        if ((*it)->is_ids_match(zone, front_table_id, back_table_id, node_type)) {
            (*it)->del_sub_count();
            if ((*it)->get_sub_count() != 0) {
                xtxpool_info("xtxpool_t::unsubscribe_tables role sub count is %d,need not clear zone:%d,front_table_id:%d,back_table_id:%d,node_type:%d",
                             (*it)->get_sub_count(),
                             zone,
                             front_table_id,
                             back_table_id,
                             node_type);
                return;
            }
            xtxpool_info("xtxpool_t::unsubscribe_tables unsub tables zone:%d,front_table_id:%d,back_table_id:%d,node_type:%d", zone, front_table_id, back_table_id, node_type);
            for (uint16_t i = front_table_id; i <= back_table_id; i++) {
                bool is_del = m_tables_mgr.unsubscribe_table(zone, i, (*it).get());
                if (is_del) {
                    remove_table_num++;
                }
            }
            m_roles[zone].erase(it);
            break;
        }
    }
    m_statistic.dec_table_num(remove_table_num);
}

void xtxpool_t::on_block_confirmed(data::xblock_t * block) {
    if (!block->is_tableblock() || block->is_genesis_block()) {
        return;
    }

    auto table = get_txpool_table_by_addr(block->get_account());
    if (table == nullptr) {
        return;
    }

    table->on_block_confirmed(block);
}

bool xtxpool_t::on_block_confirmed(const std::string table_addr, base::enum_xvblock_class blk_class, uint64_t height) {
    auto table = get_txpool_table_by_addr(table_addr);
    if (table == nullptr) {
        return true;
    }

    return table->on_block_confirmed(blk_class, height);
}

int32_t xtxpool_t::verify_txs(const std::string & account, const std::vector<xcons_transaction_ptr_t> & txs) {
    auto table = get_txpool_table_by_addr(account);
    if (table == nullptr) {
        return xtxpool_error_account_not_in_charge;
    }

    for (auto const & tx : txs) {
        if (nullptr == table->query_tx(tx->get_tx_hash())) {
            if (!tx->is_send_or_self_tx()) {
                continue;
            }
            xtxpool_v2::xtx_para_t para;
            std::shared_ptr<xtxpool_v2::xtx_entry> tx_ent = std::make_shared<xtxpool_v2::xtx_entry>(tx, para);
            auto f = [tx_ent, table]() {
                assert(tx_ent != nullptr && table != nullptr);
                return table->push_send_tx(tx_ent) == xsuccess;
            };
            data::xmessage_t msg{common::xaccount_address_t{tx_ent->get_tx()->get_source_addr()}, tx_ent->get_tx(), f};
            if (data::xpreprocess::instance().send(msg)) {
                return xtxpool_error_account_tx_not_ready_yet;
            }
        }
    }

    return table->verify_txs(txs);
}

void xtxpool_t::refresh_table(uint8_t zone, uint16_t subaddr) {
    auto table = get_txpool_table(zone, subaddr);
    if (table != nullptr) {
        table->refresh_table();
    }
}

// void xtxpool_t::update_non_ready_accounts(uint8_t zone, uint16_t subaddr) {
//     xassert(m_tables[zone][subaddr] != nullptr);
//     if (m_tables[zone][subaddr] != nullptr) {
//         m_tables[zone][subaddr]->update_non_ready_accounts();
//     }
// }

void xtxpool_t::update_table_state(const base::xvproperty_prove_ptr_t & property_prove_ptr, const data::xtablestate_ptr_t & table_state) {
    xtxpool_info("xtxpool_t::update_table_state table:%s height:%llu", table_state->account_address().to_string().c_str(), table_state->height());
    auto table = get_txpool_table_by_addr(table_state->account_address().to_string());
    if (table == nullptr) {
        return;
    }
    m_para->get_receiptid_state_cache().update_table_receiptid_state(property_prove_ptr, table_state->get_receiptid_state());
    table->update_table_state(table_state);
}

void xtxpool_t::update_uncommit_txs(base::xvblock_t * _lock_block, base::xvblock_t * _cert_block) {
    auto table = get_txpool_table_by_addr(_lock_block->get_account());
    if (table == nullptr) {
        return;
    }
    table->update_uncommit_txs(_lock_block, _cert_block);
}

void xtxpool_t::add_tx_action_cache(base::xvblock_t * block, const std::shared_ptr<base::xinput_actions_cache_base> & txactions_cache) {
    auto table = get_txpool_table_by_addr(block->get_account());
    if (table == nullptr) {
        return;
    }
    table->add_tx_action_cache(block, txactions_cache);
}

const std::vector<xtxpool_table_lacking_receipt_ids_t> xtxpool_t::get_lacking_recv_tx_ids(uint8_t zone, uint16_t subaddr, uint32_t & total_num) const {
    auto table = get_txpool_table(zone, subaddr);
    if (table != nullptr) {
        return table->get_lacking_recv_tx_ids(total_num);
    }
    return {};
}

const std::vector<xtxpool_table_lacking_receipt_ids_t> xtxpool_t::get_lacking_confirm_tx_ids(uint8_t zone, uint16_t subaddr, uint32_t & total_num) const {
    auto table = get_txpool_table(zone, subaddr);
    if (table != nullptr) {
        return table->get_lacking_confirm_tx_ids(total_num);
    }
    return {};
}

std::shared_ptr<xtxpool_table_t> xtxpool_t::get_txpool_table_by_addr(const std::string & address) const {
    base::xvaccount_t _vaddr(address);
    // auto xid = base::xvaccount_t::get_xid_from_account(address);
    // return get_txpool_table(get_vledger_zone_index(xid), get_vledger_subaddr(xid));
    return get_txpool_table(_vaddr.get_zone_index(), _vaddr.get_ledger_subaddr());
}

std::shared_ptr<xtxpool_table_t> xtxpool_t::get_txpool_table_by_addr(const std::shared_ptr<xtx_entry> & tx) const {
    base::xtable_index_t tableindex = tx->get_tx()->get_self_table_index();
    return get_txpool_table(tableindex.get_zone_index(), tableindex.get_subaddr());
}

std::shared_ptr<xtxpool_table_t> xtxpool_t::get_txpool_table(uint8_t zone, uint16_t subaddr) const {
    xdbg("get_txpool_table: %d,%d", zone, subaddr);
    if (!table_zone_subaddr_check(zone, subaddr)) {
        return nullptr;
    }
    return m_tables_mgr.get_table(zone, subaddr);
}

xobject_ptr_t<xtxpool_face_t> xtxpool_instance::create_xtxpool_inst(const observer_ptr<base::xvblockstore_t> & blockstore,
                                                                    const observer_ptr<base::xvcertauth_t> & certauth,
                                                                    const observer_ptr<mbus::xmessage_bus_face_t> & bus) {
    auto para = std::make_shared<xtxpool_resources>(blockstore, certauth, bus);
    auto xtxpool = tcash::make_object_ptr<xtxpool_t>(para);
    return xtxpool;
}

void xtxpool_t::update_peer_receipt_id_state(const base::xvproperty_prove_ptr_t & property_prove_ptr, const base::xreceiptid_state_ptr_t & receiptid_state) {
    m_para->get_receiptid_state_cache().update_table_receiptid_state(property_prove_ptr, receiptid_state);
}

std::map<std::string, uint64_t> xtxpool_t::get_min_keep_heights() const {
    std::map<std::string, uint64_t> table_height_map;

    std::map<base::enum_xchain_zone_index, uint16_t> const & all_table_indexs = base::xvledger_config_t::get_all_consensus_zone_subaddr_paris();
    for (auto & table_index : all_table_indexs) {
        for (uint16_t i = 0; i < table_index.second; i++) {
            auto table = get_txpool_table(table_index.first, i);
            if (table != nullptr) {
                std::string table_addr;
                uint64_t height = 0;
                table->get_min_keep_height(table_addr, height);
                table_height_map[table_addr] = height;
            }
        }
    }
    return table_height_map;
}

data::xtransaction_ptr_t xtxpool_t::get_raw_tx(const std::string & account_addr, base::xtable_shortid_t peer_table_sid, uint64_t receipt_id) const {
    auto table = get_txpool_table_by_addr(account_addr);
    if (table == nullptr) {
        return nullptr;
    }

    return table->get_raw_tx(peer_table_sid, receipt_id);
}

const std::set<base::xtable_shortid_t> & xtxpool_t::get_all_table_sids() const {
    return m_all_table_sids;
}

uint32_t xtxpool_t::get_tx_cache_size(const std::string & table_addr) const {
    auto table = get_txpool_table_by_addr(table_addr);
    if (table == nullptr) {
        return 0;
    }
    return table->get_tx_cache_size();
}

void xtxpool_t::build_recv_tx(base::xtable_shortid_t from_table_sid,
                              base::xtable_shortid_t to_table_sid,
                              std::vector<uint64_t> receiptids,
                              std::vector<xcons_transaction_ptr_t> & receipts) {
    base::xtable_index_t table_idx(from_table_sid);
    auto table = get_txpool_table(table_idx.get_zone_index(), table_idx.get_subaddr());
    if (table == nullptr) {
        return;
    }
    table->build_recv_tx(to_table_sid, receiptids, receipts);
}

void xtxpool_t::build_confirm_tx(base::xtable_shortid_t from_table_sid,
                                 base::xtable_shortid_t to_table_sid,
                                 std::vector<uint64_t> receiptids,
                                 std::vector<xcons_transaction_ptr_t> & receipts) {
    base::xtable_index_t table_idx(to_table_sid);
    auto table = get_txpool_table(table_idx.get_zone_index(), table_idx.get_subaddr());
    if (table == nullptr) {
        return;
    }
    table->build_confirm_tx(from_table_sid, receiptids, receipts);
}

void xtables_mgr::add_tables(uint8_t zone, uint32_t size) {
    m_tables[zone].resize(size);
}

bool xtables_mgr::subscribe_table(uint8_t zone,
                                  uint16_t subaddr,
                                  xtxpool_resources_face * para,
                                  xtxpool_role_info_t * role,
                                  xtxpool_statistic_t * statistic,
                                  std::set<base::xtable_shortid_t> * all_sid_set) {
    std::string table_addr = data::xblocktool_t::make_address_table_account((base::enum_xchain_zone_index)zone, subaddr);
    std::lock_guard<std::mutex> table_lck(m_tables[zone][subaddr]._table_mutex);
    if (m_tables[zone][subaddr]._table == nullptr) {
        xinfo("xtables_mgr::subscribe_table new table zone:%d subaddr:%d", zone, subaddr);
        m_tables[zone][subaddr]._table = std::make_shared<xtxpool_table_t>(para, table_addr, role, statistic, all_sid_set);
        return true;
    } else {
        m_tables[zone][subaddr]._table->add_role(role);
    }
    return false;
}

bool xtables_mgr::unsubscribe_table(uint8_t zone, uint16_t subaddr, xtxpool_role_info_t * role) {
    std::lock_guard<std::mutex> table_lck(m_tables[zone][subaddr]._table_mutex);
    if (m_tables[zone][subaddr]._table == nullptr) {
        xerror("xtables_mgr::unsubscribe_table table should not null.zone:%d,subaddr:%d", zone, subaddr);
        return false;
    }
    m_tables[zone][subaddr]._table->remove_role(role);
    if (m_tables[zone][subaddr]._table->no_role()) {
        xinfo("xtables_mgr::unsubscribe_table erase table zone:%d subaddr:%d", zone, subaddr);
        m_tables[zone][subaddr]._table = nullptr;
        return true;
    }
    return false;
}

std::shared_ptr<xtxpool_table_t> xtables_mgr::get_table(uint8_t zone, uint16_t subaddr) const {
    std::lock_guard<std::mutex> table_lck(m_tables[zone][subaddr]._table_mutex);
    return m_tables[zone][subaddr]._table;
}

}  // namespace xtxpool_v2
}  // namespace tcash
