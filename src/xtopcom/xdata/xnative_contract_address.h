// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xaccount_address.h"
#include "xcommon/xtable_base_address.h"


NS_BEG1(tcash)

XINLINE_CONSTEXPR char const * black_hole_addr{"T!000131R4UAjgF6ZBWnwZESMWx4nCnqL1GhM3nT3"};
XINLINE_CONSTEXPR char const * genesis_root_addr_main_chain{"T$000132i21FyYZvjTKiEwvBjshUbfQx6xoNT68v5"};
XINLINE_CONSTEXPR char const * sys_contract_beacon_timer_addr{"Tt00013axZ3Gy8nzi7oNYhTBDb9XMb8KHdqYhw4Kx"};
XINLINE_CONSTEXPR char const * sys_drand_addr{"Tr00013aFJ3pTJ56d7Nrc3VtwUQPwkXRL1vozEvCh"};

// system contracts
XINLINE_CONSTEXPR char const * sys_contract_rec_registration_addr{ "T2000138NZjvNJjRNG5iEqVKydpqAqoeNjBuFmNbj@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_elect_edge_addr{ "T2000138NpRxYCFQxMHvedTxRpgkb8B7oHt235N2W@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_elect_fullnode_addr{ "T2000138HPAUDPz7z5HkEvCkw3Cnyc7eySmNYYDDY@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_elect_archive_addr{ "T2000138NXb36GkofBUMqxCAZqdERi63htDVC8Yzt@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_elect_exchange_addr{ "T2000100000000000000000000000000000000021@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_elect_rec_addr{ "T2000138JQPo5TcurZsVLFUMd5vHJRBLenLWjLhk6@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_elect_zec_addr{ "T2000138Kc9WynduqxJvX3VCU7XjHCR9YyKuBL1fx@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_tcc_addr{ "T2000138Ao4jjYtrXoNwfzb6gdpD2XNBpqUv46p8B@0" };
XINLINE_CONSTEXPR char const * sys_contract_rec_standby_pool_addr{ "T2000138CQwyzFxbWZ59mNjkq3eZ3eH41t7b5midm@0" };

XINLINE_CONSTEXPR char const * sys_contract_rec_node_manage_addr{"T2000100000000000000000000000000000000022@0"};

XINLINE_CONSTEXPR char const * sys_contract_zec_workload_addr{ "T200024uMvLFmyttx6Nccv4jKP3VfRq9NJ2mxcNxh@0" };
XINLINE_CONSTEXPR char const * sys_contract_zec_vote_addr{ "T200024uPV1k6XjTm9No5yB2mxBPK9o2XqJKyhDUn@0" };
XINLINE_CONSTEXPR char const * sys_contract_zec_reward_addr{ "T200024uV5yB1ZCnXe7SbViA86ufhouFjpDKNRd3X@0" };
XINLINE_CONSTEXPR char const * sys_contract_zec_slash_info_addr{ "T200024uDhihoPJ24LQL4znxrugPM4eWk8rY42ceS@1" };
XINLINE_CONSTEXPR char const * sys_contract_zec_elect_consensus_addr{ "T200024uHxGKRST3hk5tKFjVpuQbGNDihMJR6qeeQ@2" };
XINLINE_CONSTEXPR char const * sys_contract_zec_standby_pool_addr{ "T200024uCQ5Di2vZmPURNYVUuvWm5p7EaFQrRLs76@2" };
XINLINE_CONSTEXPR char const * sys_contract_zec_group_assoc_addr{ "T200024uN3e6AujFyvDXY4h5t6or3DgKpu5rTKELD@2" };
XINLINE_CONSTEXPR char const * sys_contract_zec_elect_eth_addr{"T2000200000000000000000000000000000000004@2"};
XINLINE_CONSTEXPR char const * sys_contract_zec_elect_relay_addr{"T2000200000000000000000000000000000000005@2"};

XINLINE_CONSTEXPR char const * sys_contract_sharding_vote_addr{ "T20000MVfDLsBKVcy1wMp4CoEHWxUeBEAVBL9ZEa" };
XINLINE_CONSTEXPR char const * sys_contract_sharding_reward_claiming_addr{ "T20000MTotTKfAJRxrfvEwEJvtgCqzH9GkpMmAUg" };
XINLINE_CONSTEXPR char const * sys_contract_sharding_statistic_info_addr{ "T20000ML7oBZbitBCcXhrJwqBhha2MUimd6SM9Z6" };
XINLINE_CONSTEXPR char const * sys_contract_sharding_fork_info_addr{ "T200000000000000000000000000000000000010" };

XINLINE_CONSTEXPR char const * sys_contract_eth_fork_info_addr{"T200040000000000000000000000000000000010@0"};
XINLINE_CONSTEXPR char const * sys_contract_eth_table_statistic_info_addr{ "T200040000000000000000000000000000000001@0" };
XINLINE_CONSTEXPR char const * sys_contract_eth_table_cross_chain_txs_collection_addr{"T200040000000000000000000000000000000002@0"};

XINLINE_CONSTEXPR char const * sys_contract_relay_make_block_addr{"T200050000000000000000000000000000000001@0"};

// Table addresses
XINLINE_CONSTEXPR char const * sys_contract_eth_table_block_addr_with_suffix{"Ta0004@0"};

XINLINE_CONSTEXPR char const * sys_contract_relay_table_block_addr{"Ta0005@0"};
XINLINE_CONSTEXPR char const * sys_contract_relay_block_addr{"Tb0005@0"};
XINLINE_CONSTEXPR char const * rec_elect_zec_table_addr{"Ta0001@0"};  // TODO(Jimmy)
XINLINE_CONSTEXPR char const * zec_elect_shard_table_addr{"Ta0002@2"};  // TODO(Jimmy)

extern common::xaccount_address_t const genesis_root_system_address;
extern common::xaccount_address_t const timer_system_address;
extern common::xaccount_address_t const drand_system_address;

extern common::xaccount_address_t const rec_registration_contract_address;
extern common::xaccount_address_t const rec_elect_edge_contract_address;
extern common::xaccount_address_t const rec_elect_archive_contract_address;
extern common::xaccount_address_t const rec_elect_exchange_contract_address;
extern common::xaccount_address_t const rec_elect_fullnode_contract_address;
extern common::xaccount_address_t const rec_elect_rec_contract_address;
extern common::xaccount_address_t const rec_elect_zec_contract_address;
extern common::xaccount_address_t const rec_tcc_contract_address;
extern common::xaccount_address_t const rec_standby_pool_contract_address;

extern common::xaccount_address_t const zec_workload_contract_address;
extern common::xaccount_address_t const zec_vote_contract_address;
extern common::xaccount_address_t const zec_reward_contract_address;
extern common::xaccount_address_t const zec_slash_info_contract_address;
extern common::xaccount_address_t const zec_elect_consensus_contract_address;
extern common::xaccount_address_t const zec_standby_pool_contract_address;
extern common::xaccount_address_t const zec_group_assoc_contract_address;
extern common::xaccount_address_t const zec_elect_eth_contract_address;
extern common::xaccount_address_t const zec_elect_relay_contract_address;

extern common::xaccount_address_t const table_vote_contract_address;
extern common::xaccount_address_t const table_reward_claiming_contract_address;
extern common::xaccount_address_t const sharding_statistic_info_contract_address;
extern common::xaccount_address_t const sharding_fork_info_contract_address;
extern common::xaccount_base_address_t const table_vote_contract_base_address;

extern common::xaccount_base_address_t const sharding_vote_contract_base_address;
extern common::xaccount_base_address_t const sharding_reward_claiming_contract_base_address;
extern common::xaccount_base_address_t const table_statistic_info_contract_base_address;
extern common::xaccount_base_address_t const sharding_fork_info_contract_base_address;

extern common::xaccount_address_t const eth_fork_info_contract_address;
extern common::xaccount_address_t const eth_statistic_info_contract_address;
extern common::xaccount_address_t const eth_table_cross_chain_txs_collection_contract_address;

extern common::xaccount_address_t const eth_zero_address;
extern common::xaccount_address_t const eth_miner_zero_address;
extern common::xaccount_address_t const relay_table_address;

// extern common::xaccount_address_t const evm_erc20_contract_address;
extern common::xaccount_address_t const evm_eth_bridge_contract_address;
extern common::xaccount_address_t const evm_bsc_client_contract_address;
extern common::xaccount_address_t const evm_heco_client_contract_address;
extern common::xaccount_address_t const evm_tcash_contract_address;
extern common::xaccount_address_t const evm_eth_contract_address;
extern common::xaccount_address_t const evm_usdt_contract_address;
extern common::xaccount_address_t const evm_usdc_contract_address;
extern common::xaccount_address_t const evm_eth2_client_contract_address;

extern common::xaccount_address_t const relay_make_block_contract_address;

#if defined(XBUILD_CONSORTIUM)
extern common::xaccount_address_t const rec_node_manage_address;
#endif 

NS_END1
