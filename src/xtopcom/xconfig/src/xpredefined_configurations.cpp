// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if !defined(XCXX14)

#    include "xconfig/xpredefined_configurations.h"

NS_BEG2(tcash, config)

#    define XDEFINE_CONFIGURATION(NAME)                                                                                                                                            \
        constexpr char const * xtcash_##NAME##_configuration ::name;                                                                                                                 \
        constexpr xtcash_##NAME##_configuration::type xtcash_##NAME##_configuration::value

#    define XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(NAME)                                                                                                                             \
        constexpr char const * xtcash_##NAME##_onchain_goverance_parameter::name;                                                                                                    \
        constexpr tcash::xrange_t<xtcash_##NAME##_onchain_goverance_parameter::type> xtcash_##NAME##_onchain_goverance_parameter::range;                                                 \
        constexpr xonchain_goverance_parameter_classification_t xtcash_##NAME##_onchain_goverance_parameter::classification;                                                         \
        constexpr xtcash_##NAME##_onchain_goverance_parameter::type xtcash_##NAME##_onchain_goverance_parameter::value

XDEFINE_CONFIGURATION(chain_name);
XDEFINE_CONFIGURATION(root_hash);
XDEFINE_CONFIGURATION(global_timer_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(rec_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(zec_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(zone_election_trigger_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(edge_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(archive_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(exchange_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(fullnode_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cluster_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(relay_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(election_rotation_count_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cluster_election_minimum_rotation_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_auditor_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_auditor_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_validator_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_validator_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_election_committee_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_election_committee_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_auditor_rotation_count);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_edge_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_fullnode_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_archive_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(eth_election_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_eth_auditor_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_eth_auditor_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_eth_validator_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_eth_validator_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_relay_group_size);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_relay_group_size);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(eth_statistic_report_schedule_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(evm_auditor_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(evm_validator_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(evm_auditor_group_zero_workload);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(evm_validator_group_zero_workload);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(rec_standby_pool_update_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(zec_standby_pool_update_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_reading_rec_standby_pool_contract_height_step_limitation);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_reading_rec_standby_pool_contract_logic_timeout_limitation);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_validator_stake);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(auditor_nodes_per_segment);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(tx_send_timestamp_tolerance);
// XDEFINE_CONFIGURATION(receive_tx_cache_time_s);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_tcc_proposal_deposit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(tcc_proposal_expire_time);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(tcc_member);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(beacon_tx_fee);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(total_gas_shard);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_free_gas_asset);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(free_gas);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(tx_deposit_gas_exchange_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(eth_to_tcash_exchange_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(eth_gas_to_tgas_exchange_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(tcash_eth_base_price);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cpu_gas_exchange_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(usedgas_decay_cycle);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(usedgas_reset_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_tx_deposit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(unlock_gas_staked_delay_time);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(unlock_gas_deposit_delay_time);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_gas_account);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_gas_contract);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(initial_total_locked_token);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(initial_total_gas_deposit);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(fullunit_contain_of_unit_num);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(custom_property_name_max_len);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(custom_property_max_number);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(application_contract_code_max_len);

// whitelist
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(toggle_whitelist);  // default not open
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(whitelist);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(blacklist);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(additional_issue_year_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(mining_annual_ratio_from_reserve_pool);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_ratio_annual_total_reward);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_mining_annual_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_edge_deposit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_archive_deposit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_validator_deposit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_auditor_deposit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(dividend_ratio_change_interval);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_credit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_creditscore);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(initial_creditscore);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(reward_issue_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(reward_distribute_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(reward_update_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(workload_collection_interval);

XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(vote_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(validator_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(auditor_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(edge_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(archive_reward_ratio);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(governance_reward_ratio);

// mainnet node conditions
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_mainnet_active_auditors);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_mainnet_active_validators);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_mainnet_active_edges);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_mainnet_active_archives);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_mainnet_active_votes);

// xstake contracts
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_stake_votes_num);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_votes_num);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_votes_pernode_num);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_vote_nodes_num);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(votes_report_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(workload_per_tableblock);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(workload_per_tx);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_node_reward);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(min_voter_dividend);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(auditor_group_zero_workload);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(validator_group_zero_workload);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_reading_rec_reg_contract_height_step_limitation);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_reading_rec_reg_contract_logic_timeout_limitation);

// slash related
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(table_statistic_report_schedule_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(punish_collection_interval);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(punish_interval_time_block);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(slash_interval_time_block);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(backward_node_lock_duration_increment);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(max_nodedeposit_lock_duration);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(slash_nodedeposit_lock_duration_increment);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(slash_max_nodedeposit_lock_duration);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(backward_validator_slash_credit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(validator_slash_creditscore);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(backward_auditor_slash_credit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(auditor_slash_creditscore);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(punish_interval_table_block);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(slash_interval_table_block);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_ranking_publishment_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_ranking_slash_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_publishment_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_slash_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_reward_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_ranking_reward_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_awardcredit_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(sign_block_ranking_awardcredit_threshold_value);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(award_validator_credit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(award_auditor_credit);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(validator_award_creditscore);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(auditor_award_creditscore);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(contract_call_contracts_num);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_chain_contract_list);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_chain_contract_tx_list);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(cross_chain_gasprice_list);

// consortium parameters
#if defined(XBUILD_CONSORTIUM)
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(enable_node_whitelist);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(node_whitelist);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(enable_transaction_whitelist);
XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER(transaction_whitelist);
#endif 

XDEFINE_CONFIGURATION(enable_free_tgas);
XDEFINE_CONFIGURATION(evm_token_type);

/* begin of offchain parameters */
XDEFINE_CONFIGURATION(auditor_group_count);
XDEFINE_CONFIGURATION(validator_group_count);
XDEFINE_CONFIGURATION(legacy_archive_group_count);
XDEFINE_CONFIGURATION(min_edge_archive_deposit);
XDEFINE_CONFIGURATION(min_account_deposit);
XDEFINE_CONFIGURATION(zone_count);
XDEFINE_CONFIGURATION(cluster_count);
XDEFINE_CONFIGURATION(executor_max_total_sessions_service_counts);
XDEFINE_CONFIGURATION(executor_max_session_service_counts);
XDEFINE_CONFIGURATION(executor_session_time_interval);
XDEFINE_CONFIGURATION(executor_max_sessions);
XDEFINE_CONFIGURATION(recv_tx_cache_window);
XDEFINE_CONFIGURATION(config_property_alias_name_max_len);
XDEFINE_CONFIGURATION(account_send_queue_tx_max_num);
XDEFINE_CONFIGURATION(edge_max_msg_packet_size);
XDEFINE_CONFIGURATION(leader_election_round);
XDEFINE_CONFIGURATION(unitblock_confirm_tx_batch_num);
XDEFINE_CONFIGURATION(unitblock_recv_transfer_tx_batch_num);
XDEFINE_CONFIGURATION(unitblock_send_transfer_tx_batch_num);
XDEFINE_CONFIGURATION(tableblock_batch_tx_max_num);
XDEFINE_CONFIGURATION(tableblock_batch_unitblock_max_num);
XDEFINE_CONFIGURATION(fulltable_interval_block_num);

XDEFINE_CONFIGURATION(local_blacklist);
XDEFINE_CONFIGURATION(local_whitelist);
XDEFINE_CONFIGURATION(local_toggle_whitelist);

XDEFINE_CONFIGURATION(slash_fulltable_interval);
XDEFINE_CONFIGURATION(slash_table_split_num);
XDEFINE_CONFIGURATION(prune_reserve_number);

XDEFINE_CONFIGURATION(evm_relay_txs_collection_interval);
XDEFINE_CONFIGURATION(relayblock_batch_tx_max_num);
XDEFINE_CONFIGURATION(max_relay_poly_interval);
XDEFINE_CONFIGURATION(max_relay_tx_block_interval);
XDEFINE_CONFIGURATION(max_relay_tx_block_interval_fast);
XDEFINE_CONFIGURATION(max_relay_poly_interval_fast);
XDEFINE_CONFIGURATION(relayblock_batch_tx_max_num_fast);

/* beginning of development parameters */
XDEFINE_CONFIGURATION(http_port);
XDEFINE_CONFIGURATION(grpc_port);
XDEFINE_CONFIGURATION(dht_port);
XDEFINE_CONFIGURATION(msg_port);
XDEFINE_CONFIGURATION(ws_port);
XDEFINE_CONFIGURATION(evm_port);
XDEFINE_CONFIGURATION(chain_id);
XDEFINE_CONFIGURATION(network_id);
XDEFINE_CONFIGURATION(log_level);
XDEFINE_CONFIGURATION(log_path);
XDEFINE_CONFIGURATION(db_path);
XDEFINE_CONFIGURATION(ip);
XDEFINE_CONFIGURATION(evm_json_rpc_port);
/* end of development parameters */

/* end of offchain parameters */
XDEFINE_CONFIGURATION(node_p2p_port);
XDEFINE_CONFIGURATION(p2p_endpoints);
XDEFINE_CONFIGURATION(p2p_url_endpoints);

XDEFINE_CONFIGURATION(sync_table_state_height_gap);
XDEFINE_CONFIGURATION(keep_table_states_max_num);
XDEFINE_CONFIGURATION(prune_table_state_diff);
XDEFINE_CONFIGURATION(prune_table_state_max);

XDEFINE_CONFIGURATION(table_fork_info_interval);

#    undef XDEFINE_ONCHAIN_GOVERNANCE_PARAMETER
#    undef XDEFINE_CONFIGURATION

NS_END2

#endif
