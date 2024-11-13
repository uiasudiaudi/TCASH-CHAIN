// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xvm/xsystem_contracts/deploy/xcontract_deploy.h"

#include "xbase/xutl.h"
#include "xcommon/xnode_type.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdata/xgenesis_data.h"
#include "xdata/xnative_contract_address.h"

using namespace tcash::common;
using namespace tcash::data;

NS_BEG2(tcash, contract)

xtcash_contract_deploy & xtcash_contract_deploy::instance() {
    static xtcash_contract_deploy inst;
    return inst;
}

void xtcash_contract_deploy::deploy_sys_contracts() {
#if !defined(XBUILD_CONSORTIUM)
    xdbg("[xtcash_contract_deploy::deploy_sys_contracts]");
    deploy(common::xaccount_address_t{sys_contract_rec_registration_addr}, xnode_type_t::committee, "all", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_rec_standby_pool_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string{sys_contract_beacon_timer_addr} + ",on_timer,C," + config::xrec_standby_pool_update_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_workload_addr},
           xnode_type_t::zec,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xworkload_collection_interval_onchain_goverance_parameter_t::name);
    deploy(common::xaccount_address_t{sys_contract_zec_vote_addr}, xnode_type_t::zec, "all", enum_broadcast_policy_t::normal);
    deploy(common::xaccount_address_t{sys_contract_zec_reward_addr},
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xreward_update_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_tcc_addr}, xnode_type_t::committee, "all", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_fullnode_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xfullnode_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_edge_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xedge_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_archive_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xarchive_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_exchange_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xexchange_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_rec_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xrec_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_zec_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xzec_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_elect_consensus_addr},
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xzone_election_trigger_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_standby_pool_addr},
           xnode_type_t::zec,
           "zec",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xzec_standby_pool_update_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_group_assoc_addr}, xnode_type_t::zec, "all", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_sharding_statistic_info_addr},
           xnode_type_t::consensus_validator,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",report_summarized_statistic_info,C," + config::xtable_statistic_report_schedule_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_sharding_vote_addr},
           xnode_type_t::consensus_validator,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C,1");

    // deploy(common::xaccount_address_t{sys_contract_eth_table_statistic_info_addr},
    //        xnode_type_t::evm_validator,
    //        "",
    //        enum_broadcast_policy_t::normal,
    //        std::string(sys_contract_beacon_timer_addr) + ",report_summarized_statistic_info,C," + config::xeth_statistic_report_schedule_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_slash_info_addr},
           xnode_type_t::zec,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",do_unqualified_node_slash,C," + config::xpunish_collection_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_sharding_reward_claiming_addr}, xnode_type_t::consensus_validator, "", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t::build_from(sharding_fork_info_contract_base_address),
           xnode_type_t::consensus_validator,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xtable_fork_info_interval_configuration_t::name);

    deploy(eth_fork_info_contract_address,
           xnode_type_t::evm_validator,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xtable_fork_info_interval_configuration_t::name);

    deploy(zec_elect_eth_contract_address,
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xeth_election_interval_onchain_goverance_parameter_t::name);
    deploy(zec_elect_relay_contract_address,
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xrelay_election_interval_onchain_goverance_parameter_t::name);
    deploy(eth_table_cross_chain_txs_collection_contract_address,
           xnode_type_t::evm_validator,
           "",
           enum_broadcast_policy_t::normal);
    deploy(relay_make_block_contract_address, xnode_type_t::relay, "all", enum_broadcast_policy_t::normal, std::string(sys_contract_beacon_timer_addr) + ",_,C");
#else 
    xdbg("[xtcash_contract_deploy::deploy_sys_consortium_contracts]");
    
    deploy(common::xaccount_address_t { sys_contract_sharding_statistic_info_addr },
        xnode_type_t::consensus_validator,
        "",
        enum_broadcast_policy_t::normal,
        std::string(sys_contract_beacon_timer_addr) + ",report_summarized_statistic_info,C," + config::xtable_statistic_report_schedule_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t { sys_contract_eth_table_statistic_info_addr },
        xnode_type_t::evm_validator,
        "",
        enum_broadcast_policy_t::normal,
        std::string(sys_contract_beacon_timer_addr) + ",report_summarized_statistic_info,C," + config::xeth_statistic_report_schedule_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t { sys_contract_zec_reward_addr },
        xnode_type_t::zec,
        "all",
        enum_broadcast_policy_t::normal,
        std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xreward_update_interval_onchain_goverance_parameter_t::name);
    

    deploy(common::xaccount_address_t { sys_contract_sharding_reward_claiming_addr }, xnode_type_t::consensus_validator, "", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_rec_registration_addr}, xnode_type_t::committee, "all", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_rec_node_manage_addr}, xnode_type_t::committee, "all", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_rec_standby_pool_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string{sys_contract_beacon_timer_addr} + u8",on_timer,C," + config::xrec_standby_pool_update_interval_onchain_goverance_parameter_t::name);


    deploy(common::xaccount_address_t{sys_contract_rec_tcc_addr}, xnode_type_t::committee, "all", enum_broadcast_policy_t::normal);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_fullnode_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xfullnode_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_edge_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xedge_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_archive_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xarchive_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_exchange_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xexchange_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_rec_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xrec_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_rec_elect_zec_addr},
           xnode_type_t::committee,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xzec_election_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_elect_consensus_addr},
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xzone_election_trigger_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_standby_pool_addr},
           xnode_type_t::zec,
           "zec",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xzec_standby_pool_update_interval_onchain_goverance_parameter_t::name);

    deploy(common::xaccount_address_t{sys_contract_zec_group_assoc_addr}, xnode_type_t::zec, "all", enum_broadcast_policy_t::normal);

    deploy(sharding_fork_info_contract_address,
           xnode_type_t::consensus_validator,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xtable_fork_info_interval_configuration_t::name);

    deploy(eth_fork_info_contract_address,
           xnode_type_t::evm_validator,
           "",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xtable_fork_info_interval_configuration_t::name);

    deploy(zec_elect_eth_contract_address,
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xeth_election_interval_onchain_goverance_parameter_t::name);
    deploy(zec_elect_relay_contract_address,
           xnode_type_t::zec,
           "all",
           enum_broadcast_policy_t::normal,
           std::string(sys_contract_beacon_timer_addr) + ",on_timer,C," + config::xrelay_election_interval_onchain_goverance_parameter_t::name);
    deploy(eth_table_cross_chain_txs_collection_contract_address,
           xnode_type_t::evm_validator,
           "",
           enum_broadcast_policy_t::normal);
    deploy(relay_make_block_contract_address, xnode_type_t::relay, "all", enum_broadcast_policy_t::normal, std::string(sys_contract_beacon_timer_addr) + ",_,C");
#endif
    
}

bool xtcash_contract_deploy::deploy(common::xaccount_address_t const & address,
                                  common::xnode_type_t _roles,
                                  const std::string & _broadcast_types,
                                  enum_broadcast_policy_t _broadcast_policy,
                                  const std::string & _block_monitors) {
    auto it = m_info_map.find(address);
    if (it != m_info_map.end()) {
        return false;
    }

    xcontract_info_t * info = new xcontract_info_t(address, _roles);  // force : no broadcast allowed
    if (is_sys_contract_address(common::xaccount_address_t{address})) {
        xdbg("[xtcash_contract_deploy::deploy] add monitor for %s", address.to_string().c_str());
        // boradcast policy
        info->broadcast_policy = _broadcast_policy;
        info->broadcast_types = str_to_broadcast_types(_broadcast_types);
        for (auto & str : str_to_list(_block_monitors)) {
            std::vector<std::string> monitor_info = str_to_list(str, ',');
            if (monitor_info.size() == 1) {                                                   // only timer contract allowed
                if (address == common::xaccount_address_t{sys_contract_beacon_timer_addr} &&  // check address
                    common::has<common::xnode_type_t::committee>(_roles)) {                   // add role
                    info->add_local_timer_monitor(common::xaccount_address_t{sys_local_timer_addr}, monitor_info[0]);
                }
            } else if (monitor_info.size() == 3) {
                info->add_block_monitor(
                    common::xaccount_address_t{monitor_info[0]}, monitor_info[1], monitor_info[2] == "NC" ? enum_call_action_way_t::direct : enum_call_action_way_t::consensus);
            } else if (monitor_info.size() == 4) {
                // monitor_info[0]: monitor address. timer event src;
                // monitor_info[1]: timer event handler name;
                // monitor_info[2]: do validation (consensus) or not
                // monitor_info[4]: timer interval
                uint32_t time_interval{};
                std::string conf_interval{};
                if (std::all_of(monitor_info[3].begin(), monitor_info[3].end(), [](const char & c) { return '0' <= c && c <= '9'; })) {
                    time_interval = base::xstring_utl::touint32(monitor_info[3]);
                } else {
                    conf_interval = monitor_info[3];
                }
                info->add_timer_monitor(common::xaccount_address_t{monitor_info[0]},
                                        monitor_info[1],
                                        monitor_info[2] == "NC" ? enum_call_action_way_t::direct : enum_call_action_way_t::consensus,
                                        time_interval,
                                        conf_interval);
                xdbg("[xtcash_contract_deploy::deploy] add timer monitor for %s, %s, interval %d, config interval %s",
                     address.to_string().c_str(),
                     monitor_info[0].c_str(),
                     time_interval,
                     conf_interval.c_str());
            }
        }
    }
    xdbg("[xtcash_contract_deploy::deploy] add done for %s", address.to_string().c_str());
    m_info_map[address] = info;
    return true;
}

xcontract_info_t * xtcash_contract_deploy::find(common::xaccount_address_t const & address) {
    auto it = m_info_map.find(address);
    if (it != m_info_map.end()) {
        return it->second;
    }
    return nullptr;
}

std::unordered_map<common::xaccount_address_t, xcontract_info_t *> const & xtcash_contract_deploy::get_map() const noexcept {
    return m_info_map;
}

void xtcash_contract_deploy::clear() {
    for (auto & pair : m_info_map) {
        delete pair.second;
    }
    m_info_map.clear();
}

std::vector<std::string> xtcash_contract_deploy::str_to_list(const std::string & str, const char sep) {
    std::vector<std::string> list;
    base::xstring_utl::split_string(str, sep, list);
    return list;
}

common::xnode_type_t xtcash_contract_deploy::str_to_broadcast_types(const std::string & str) {
    common::xnode_type_t types{common::xnode_type_t::invalid};
    for (auto & s : str_to_list(str)) {
        if (s == "rec") {
            types |= common::xnode_type_t::committee;
        } else if (s == "zec") {
            types |= common::xnode_type_t::zec;
        } else if (s == "arc") {
            types |= common::xnode_type_t::storage;
        } else if (s == "all") {
            types |= common::xnode_type_t::real_part_mask;
        }
    }
    return types;
}

NS_END2
