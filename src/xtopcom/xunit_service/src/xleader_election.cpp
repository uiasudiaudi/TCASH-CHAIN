// Copyright (c) 2017-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xunit_service/xleader_election.h"

#include "xcommon/xfts.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdata/xnative_contract_address.h"
#include "xdata/xtable_id_mapping.h"
#include "xelection/xdata_accessor_error.h"
#include "xunit_service/xcons_utl.h"

#include <cinttypes>

// for xvip compare
static bool operator==(const xvip2_t & lhs, const xvip2_t & rhs) {
    return lhs.high_addr == rhs.high_addr && lhs.low_addr == rhs.low_addr;
}

NS_BEG2(tcash, xunit_service)

// load manager tables
int32_t xelection_cache_imp::get_tables(const xvip2_t & xip, std::vector<base::xtable_index_t> * tables) {
    auto zone_id = get_zone_id_from_xip2(xip);
    if (zone_id == base::enum_chain_zone_beacon_index) {
        for (uint8_t i = 0; i < MAIN_CHAIN_REC_TABLE_USED_NUM; i++) {
            tables->push_back({base::enum_chain_zone_beacon_index, i});
        }
        return tables->size();
    } else if (zone_id == base::enum_chain_zone_zec_index) {
        for (uint8_t i = 0; i < MAIN_CHAIN_ZEC_TABLE_USED_NUM; i++) {
            tables->push_back({base::enum_chain_zone_zec_index, i});
        }
    } else if (zone_id == base::enum_chain_zone_evm_index) {
        for (uint8_t i = 0; i < MAIN_CHAIN_EVM_TABLE_USED_NUM; i++) {
            tables->push_back({base::enum_chain_zone_evm_index, i});
        }
    } else if (zone_id == base::enum_chain_zone_relay_index) {
        for (uint8_t i = 0; i < MAIN_CHAIN_RELAY_TABLE_USED_NUM; ++i) {
            tables->push_back({base::enum_chain_zone_relay_index, i});
        }
    } else {
        {
            xvip2_t tmp = get_group_xip2(xip);
            std::lock_guard<std::mutex> lock(m_mutex);
            auto iter = m_elect_data.find(tmp);
            if (iter != m_elect_data.end()) {
                auto tables_v = iter->second.tables;
                for(auto iter = tables_v.begin(); iter != tables_v.end(); iter++) {
                    tables->push_back({base::enum_chain_zone_consensus_index, (uint8_t)*iter});
                }
                return tables->size();
            }
        }
    }
    return 0;
}

// load election data from db
int32_t xelection_cache_imp::get_election(const xvip2_t & xip, elect_set * elect_data, bool bself) {
    {
        xvip2_t tmp = get_group_xip2(xip);
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_elect_data.find(tmp);
        if (iter != m_elect_data.end()) {
            elect_data->insert(elect_data->begin(), iter->second.self.begin(), iter->second.self.end());
            return elect_data->size();
        }
    }

    return 0;
}

// load parent data
int32_t xelection_cache_imp::get_parent_election(const xvip2_t & xip, elect_set * elect_data) {
    {
        xvip2_t tmp = get_group_xip2(xip);
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_elect_data.find(tmp);
        if (iter != m_elect_data.end()) {
            elect_data->insert(elect_data->begin(), iter->second.parent.begin(), iter->second.parent.end());
            return elect_data->size();
        }
    }
    return 0;
}

// load parent data
int32_t xelection_cache_imp::get_group_election(const xvip2_t & xip, int32_t group_id, elect_set * elect_data) {
    {
        xvip2_t tmp = get_group_xip2(xip);
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_elect_data.find(tmp);
        if (iter != m_elect_data.end()) {
            auto child_iter = iter->second.children.find(group_id);
            if (child_iter != iter->second.children.end()) {
                elect_data->insert(elect_data->begin(), child_iter->second.begin(), child_iter->second.end());
            }
            return elect_data->size();
        }
    }
    return 0;
}

// add elect data
bool xelection_cache_imp::add(const xvip2_t & xip,
                              const elect_set & elect_data,
                              const std::vector<uint16_t> & tables,
                              const elect_set & parent,
                              std::map<int32_t, elect_set> children) {
    {
        xvip2_t tmp = get_group_xip2(xip);
        std::lock_guard<std::mutex> lock(m_mutex);
        xelect_item item{tables, elect_data, parent, children};
        m_elect_data[tmp] = item;
        xunit_dbg_info("xelection_cache_imp::add xip=%s,group=%s,parent_size=%d,children_size=%d",
            xcons_utl::xip_to_hex(xip).c_str(), xcons_utl::xip_to_hex(tmp).c_str(), parent.size(), children.size());
#ifdef DEBUG
        for (auto & v : children) {
            if (v.second.size() == 0) {
                xerror("xelection_cache_imp::add children xip=%s,group=%s,parent_size=%d,children_size=%d,groupid=%d,child_electsize=%d",
                    xcons_utl::xip_to_hex(xip).c_str(), xcons_utl::xip_to_hex(tmp).c_str(), parent.size(), children.size(), v.first, v.second.size());
            }
        }
#endif
    }
    return true;
}

// erase cached elect data
bool xelection_cache_imp::erase(const xvip2_t & xip) {
    {
        xvip2_t tmp = get_group_xip2(xip);
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_elect_data.find(tmp);
        if (iter != m_elect_data.end()) {
            m_elect_data.erase(iter);
        }
        xunit_dbg_info("xelection_cache_imp::erase xip=%s,group=%s", xcons_utl::xip_to_hex(xip).c_str(), xcons_utl::xip_to_hex(tmp).c_str());
    }
    return true;
}

xvip2_t xelection_cache_imp::get_group_xip2(xvip2_t const & xip) {
    xvip2_t tmp = xip;
    reset_node_id_to_xip2(tmp);
    set_node_id_to_xip2(tmp, 0xFFF);
    return tmp;
}

xrandom_leader_election::xrandom_leader_election(const xobject_ptr_t<base::xvblockstore_t> & block_store, const std::shared_ptr<xelection_cache_face> & face)
  : m_blockstore(block_store), m_elector(face) {}

xvip2_t get_leader(const xelection_cache_face::elect_set & nodes, const common::xelection_round_t& version, uint64_t factor) {
    assert(version.has_value());
    if (nodes.empty()) {
        xunit_warn("[xunitservice] leader_election candidates empty");
        return xvip2_t{(uint64_t)-1, (uint64_t)-1};
    }

    std::vector<common::xfts_merkle_tree_t<xvip2_t>::value_type> candidates;
    std::vector<common::xfts_merkle_tree_t<xvip2_t>::value_type> reliable_candidates;
    uint32_t const default_leader_election_round = XGET_CONFIG(leader_election_round);
    auto const min_creditscore = XGET_ONCHAIN_GOVERNANCE_PARAMETER(min_creditscore);

    for (auto const & node_datum : nodes) {
        if (version.value() >= (node_datum.election_info.joined_epoch().value() + default_leader_election_round)) {
            if (common::has<common::xnode_type_t::consensus>(node_datum.address.type()) && !node_datum.election_info.genesis() &&
                node_datum.election_info.raw_credit_score() <= min_creditscore) {
                candidates.push_back({static_cast<common::xstake_t>(node_datum.election_info.comprehensive_stake() + 1), static_cast<xvip2_t>(node_datum.address.xip2())});
                xdbg("account %s is moved to low priority leader election candidate pool. account credit score %" PRIu64 " min credit score %" PRIu64,
                     node_datum.address.to_string().c_str(),
                     node_datum.election_info.raw_credit_score(),
                     min_creditscore);
                continue;
            }

            xdbg("account %s is moved to high priority leader election candidate pool. account credit score %" PRIu64 " min credit score %" PRIu64,
                 node_datum.address.to_string().c_str(),
                 node_datum.election_info.raw_credit_score(),
                 min_creditscore);
            reliable_candidates.push_back(
                {static_cast<common::xstake_t>(node_datum.election_info.comprehensive_stake() + 1), static_cast<xvip2_t>(node_datum.address.xip2())});
        } else {
            // <version, joined_version> = <0, 0>, <1, 0>, <N, N-1>, <N, N>
            if (node_datum.election_info.joined_epoch().value() == 0) {
                xdbg("account is moved to high priority leader election candidate pool %s. account credit score %" PRIu64 " min credit score %" PRIu64,
                     node_datum.address.to_string().c_str(),
                     node_datum.election_info.raw_credit_score(),
                     min_creditscore);
                reliable_candidates.push_back({static_cast<common::xstake_t>(node_datum.election_info.comprehensive_stake() + 1), static_cast<xvip2_t>(node_datum.address.xip2())});
            } else {
                xdbg("account is moved to low priority leader election candidate pool %s. account credit score %" PRIu64 " min credit score %" PRIu64,
                     node_datum.address.to_string().c_str(),
                     node_datum.election_info.raw_credit_score(),
                     min_creditscore);
                candidates.push_back({static_cast<common::xstake_t>(node_datum.election_info.comprehensive_stake() + 1), static_cast<xvip2_t>(node_datum.address.xip2())});
            }
        }
    }

    std::vector<common::xfts_merkle_tree_t<xvip2_t>::value_type> leaders;
    if (!reliable_candidates.empty()) {
        leaders = common::select<xvip2_t>(reliable_candidates, factor, 1);
    } else {
        leaders = common::select<xvip2_t>(candidates, factor, 1);
    }

    return leaders[0].second;
}

const xvip2_t xrandom_leader_election::get_leader_xip(uint64_t viewId,
                                                      const std::string & account,
                                                      base::xvblock_t * prev_block,
                                                      const xvip2_t & local,
                                                      const xvip2_t & candidate,
                                                      const common::xelection_round_t & version,
                                                      uint16_t rotate_mode) {
    // TODO(justin): leader maybe cross group for auditor & validator
    // if (!is_xip2_group_equal(local, candidate)) {
    //     xunit_warn("[xunitservice] recv invalid candidiate %s at %s not equal group", xcons_utl::xip_to_hex(candidate).c_str(), xcons_utl::xip_to_hex(local).c_str());
    //     return false;
    // }

    uint64_t random = viewId + base::xvaccount_t::get_xid_from_account(account);
    xelection_cache_face::elect_set elect_set;
    if (m_elector != nullptr) {
        auto len = m_elector->get_election(local, &elect_set);
        if (len <= 0) {
            xunit_warn("[xunitservice] recv invalid candidiate %s at %s", xcons_utl::xip_to_hex(candidate).c_str(), xcons_utl::xip_to_hex(local).c_str());
            return {};
        }
    }

    if (rotate_mode != enum_rotate_mode_no_rotate) {
        auto str = prev_block->get_block_hash();
        random = base::xhash64_t::digest(str) + random;
        // auditor will always trust validator leader
        if (xcons_utl::is_auditor(local)) {
            if (xcons_utl::is_validator(candidate)) {
                return local;
            } else {
                return {};
            }
        }
    }

    xvip2_t leader_xip = get_leader(elect_set, version, random);
    xunit_dbg_info("[xunitservice] xrandom_leader_election::get_leader_xip account=%s,viewid=%ld,random=%ld,leader_xip=%s,local=%s,candidate=%s",
                   account.c_str(),
                   viewId,
                   random,
                   xcons_utl::xip_to_hex(leader_xip).c_str(),
                   xcons_utl::xip_to_hex(local).c_str(),
                   xcons_utl::xip_to_hex(candidate).c_str());
    return leader_xip;
}

xelection_cache_face * xrandom_leader_election::get_election_cache_face() {
    return m_elector.get();
}

int to_elect_set(const std::map<common::xslot_id_t, data::xnode_info_t> & node_map, xelection_cache_imp::elect_set & elect_set) {
    if (node_map.empty()) {
        return 0;
    }
    assert(elect_set.empty());

    elect_set.reserve(node_map.size());
    std::transform(std::begin(node_map), std::end(node_map), std::back_inserter(elect_set), [](std::pair<common::xslot_id_t const, data::xnode_info_t> const & datum) {
        return tcash::get<data::xnode_info_t>(datum);
    });

    std::sort(std::begin(elect_set), std::end(elect_set), [](const data::xnode_info_t & a, const data::xnode_info_t & b) -> bool { return a.address.slot_id() < b.address.slot_id(); });
    return elect_set.size();
}

bool xelection_wrapper::on_network_start(xelection_cache_face * p_election,
                                         const xvip2_t & xip,
                                         const std::shared_ptr<vnetwork::xvnetwork_driver_face_t> & network,
                                         const election::cache::xdata_accessor_face_t * elect_cache_ptr,
                                         uint64_t chain_time) {
    std::vector<uint16_t> tables = data::get_table_ids(network->address().group_address(), network->parent_group_address().group_id());
    if (tables.empty()) {
        return false;
    }
    // auto                           elect_data = network->neighbors_info2();
    auto address = network->address();
    auto group_address = address.group_address();

    // neighbours
    std::error_code ec{election::xdata_accessor_errc_t::success};
    auto elect_data = elect_cache_ptr->sharding_nodes(group_address, address.election_round(), ec);
    if (ec) {
        xwarn("xelection_wrapper::on_network_start failed to get group nodes. err msg %s", ec.message().c_str());
        return false;
    }
    xelection_cache_imp::elect_set elect_set_;
    to_elect_set(elect_data, elect_set_);

    ec.clear();
    auto parent_cluster_address = elect_cache_ptr->parent_address(group_address, address.election_round(), ec);
    auto parent_address = parent_cluster_address.sharding_address();
    ec.clear();
    auto parent_elect_data = elect_cache_ptr->sharding_nodes(parent_address, address.election_round(), ec);
    xelection_cache_imp::elect_set parent_set_;
    to_elect_set(parent_elect_data, parent_set_);

    // children data
    std::map<int32_t, xelection_cache_imp::elect_set> child_map;
    ec.clear();
    auto group_ele = elect_cache_ptr->group_element(group_address, address.election_round(), ec);
    if (!ec && group_ele != nullptr) {
        // common::xlogic_time_t       logic_time_now{chain_time};
        ec.clear();
        auto children = group_ele->associated_child_groups(common::xjudgement_day, ec);
        std::map<int32_t, uint64_t> group_versions;
        for (auto iter = children.begin(); iter != children.end(); iter++) {
            auto group_id = (*iter)->group_id().value();
            auto child_address = (*iter)->address();
            auto child_shard_address = child_address.sharding_address();
            ec.clear();
            auto child_elect_data = elect_cache_ptr->sharding_nodes(child_shard_address, child_address.election_round(), ec);
            xelection_cache_imp::elect_set children_set_;
            to_elect_set(child_elect_data, children_set_);
            auto version = child_address.election_round().value();
            auto group_iter = child_map.find(group_id);
            if (group_iter != child_map.end()) {
                if (group_versions[group_id] < version) {
                    child_map[group_id] = children_set_;
                    group_versions[group_id] = version;
                }
            } else {
                child_map[group_id] = children_set_;
                group_versions[group_id] = version;
            }
        }
    }
    // auditor will exchange to as validator election
    xunit_info("[xunit_service] add election cache %s address:%s, node size:%d, parent:%d, child:%d",
               xcons_utl::xip_to_hex(xip).c_str(),
               address.to_string().c_str(),
               elect_set_.size(),
               parent_set_.size(),
               child_map.size());
    return p_election->add(xip, elect_set_, tables, parent_set_, child_map);
}

bool xelection_wrapper::on_network_destory(xelection_cache_face * p_election, const xvip2_t & xip) {
    return p_election->erase(xip);
}

xrotate_leader_election::xrotate_leader_election(const observer_ptr<base::xvblockstore_t> & block_store, const std::shared_ptr<xelection_cache_face> & face)
  : m_blockstore(block_store), m_elector(face) {}

bool xrotate_leader_election::is_rotate_xip(const xvip2_t & local) {
    bool rotate = false;
    auto zone_id = get_zone_id_from_xip2(local);
    if (zone_id == base::enum_chain_zone_consensus_index || zone_id == base::enum_chain_zone_evm_index) {
        rotate = true;
    }
    return rotate;
}

const xvip2_t xrotate_leader_election::get_leader_xip(uint64_t viewId,
                                                      const std::string & account,
                                                      base::xvblock_t * prev_block,
                                                      const xvip2_t & local,
                                                      const xvip2_t & candidate,
                                                      const common::xelection_round_t & version,
                                                      uint16_t rotate_mode) {
    uint64_t random = viewId + base::xvaccount_t::get_xid_from_account(account);
    xelection_cache_face::elect_set elect_set;
    // bool leader = false;
    bool prev_is_validator = true;

    if (rotate_mode == enum_rotate_mode_no_rotate) {
        m_elector->get_election(local, &elect_set);
    } else {
        bool rotate = is_rotate_xip(local);
        if (rotate) {
// get prev block leader
#if 0
            if (rotate_mode == enum_rotate_mode_rotate_by_last_block) {
                auto validator = prev_block->get_cert()->get_validator();
                if (get_node_id_from_xip2(validator) == 0x3FF) {  // 0x3FF is a group node xip
                    prev_is_validator = false;
                }
            }
#endif
            if (rotate_mode == enum_rotate_mode_rotate_by_view_id) {
                // viewid determine where leader is from
                prev_is_validator = viewId & 0x1;
            }

            // prev block is validator as leader
            if (prev_is_validator) {
                // perv block is validator as leader
                if (xcons_utl::is_auditor(local)) {
                    // current is auditor load self elect data as candidates
                    m_elector->get_election(local, &elect_set);
                } else {
                    // validator load parent elect data as candidates
                    m_elector->get_parent_election(local, &elect_set);
                }
            } else {
                // perv block is auditor as leader
                if (xcons_utl::is_auditor(local)) {
                    // current is auditor get children as candidates
                    auto group_id = xcons_utl::get_groupid_by_account(local, account);
                    m_elector->get_group_election(local, group_id, &elect_set);
                } else {
                    // validator will load self elect data as candidates
                    m_elector->get_election(local, &elect_set);
                }
            }
        } else {
            // get validator as candidates
            m_elector->get_election(local, &elect_set, true);
        }
    }

    xvip2_t leader_xip = get_leader(elect_set, version, random);
    xunit_dbg_info("xrotate_leader_election::get_leader_xip account=%s,viewid=%ld,random=%ld,leader_xip=%s,local=%s,candidate=%s,prev_validator=%d,electsize:%d",
                   account.c_str(),
                   viewId,
                   random,
                   xcons_utl::xip_to_hex(leader_xip).c_str(),
                   xcons_utl::xip_to_hex(local).c_str(),
                   xcons_utl::xip_to_hex(candidate).c_str(),
                   prev_is_validator,
                   elect_set.size());
    return leader_xip;
}

xelection_cache_face * xrotate_leader_election::get_election_cache_face() {
    return m_elector.get();
}

NS_END2
