// Copyright (c) 2018-Present Telos Foundation & contributors
// taylor.wei@tcashnetwork.org
// Licensed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <cinttypes>
#include "xmetrics/xmetrics.h"
#include "xvblockpruner.h"
#include "xdata/xnative_contract_address.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"

namespace tcash
{
    namespace store
    {
        xvblockprune_impl::xvblockprune_impl(base::xvdbstore_t & xdb_api)
        {
            m_xvdb_ptr = &xdb_api;
            xassert(enum_reserved_blocks_count > 0);

            m_prune_contract.clear();
            m_prune_contract[sys_contract_rec_registration_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_elect_edge_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_elect_fullnode_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_elect_archive_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_elect_exchange_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_elect_rec_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_elect_zec_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_tcc_addr] = enum_prune_none;
            m_prune_contract[sys_contract_rec_standby_pool_addr] = enum_prune_none;

            m_prune_contract[sys_contract_zec_workload_addr] = enum_prune_none;
            m_prune_contract[sys_contract_zec_vote_addr] = enum_prune_fullunit;
            m_prune_contract[sys_contract_zec_reward_addr] = enum_prune_fullunit;
            m_prune_contract[sys_contract_zec_slash_info_addr] = enum_prune_fullunit;
            m_prune_contract[sys_contract_zec_elect_consensus_addr] = enum_prune_none;
            m_prune_contract[sys_contract_zec_standby_pool_addr] = enum_prune_none;
            m_prune_contract[sys_contract_zec_group_assoc_addr] = enum_prune_none;

            for (auto index = 0; index < enum_vledger_const::enum_vbucket_has_tables_count; ++index) {
                std::string addr;
                addr = std::string(sys_contract_sharding_vote_addr) + "@" + std::to_string(index);
                m_prune_contract[addr] = enum_prune_fullunit;
                addr = std::string(sys_contract_sharding_reward_claiming_addr) + "@" + std::to_string(index);
                m_prune_contract[addr] = enum_prune_fullunit;
                addr = std::string(sys_contract_sharding_statistic_info_addr) + "@" + std::to_string(index);
                m_prune_contract[addr] = enum_prune_fullunit;
            }
        }

        xvblockprune_impl::~xvblockprune_impl()
        {
            xwarn("~xvblockprune_impl");
        }
    
        bool  xvblockprune_impl::recycle(const base::xvbindex_t * block) //recyle one block
        {
            if(NULL == block)
               return false;
               
            return false;
        }
    
        bool  xvblockprune_impl::recycle(const std::vector<base::xvbindex_t*> & mblocks) //recycle multiple blocks
        {
            bool result = false;
            for(auto block : mblocks)
            {
                if(recycle(block))
                    result = true;
            }
            return result;
        }
    
        bool  xvblockprune_impl::recycle(const base::xvaccount_t & account_obj,base::xblockmeta_t & account_meta)//recylce any qualified blocks under account
        {           
            if(account_obj.is_user_address()) {
                auto zone_id = account_obj.get_zone_index();
                // if consensus zone
                if ((zone_id == base::enum_chain_zone_zec_index) || (zone_id == base::enum_chain_zone_beacon_index)) {
                    return false;
                }
                if ((zone_id == base::enum_chain_zone_evm_index) || (zone_id == base::enum_chain_zone_relay_index)) {
                    return false;
                }

                return recycle_unit(account_obj,account_meta);
            } else if(account_obj.is_table_address()) {
                auto zone_id = account_obj.get_zone_index();
                if ((zone_id == base::enum_chain_zone_evm_index) || (zone_id == base::enum_chain_zone_relay_index)) {
                    return false;
                }
                return recycle_table(account_obj,account_meta);
            } else if(account_obj.is_contract_address()) {
                return recycle_contract(account_obj,account_meta);
            } else if(account_obj.is_timer_address()) {
                return recycle_timer(account_obj,account_meta);
            } else if(account_obj.is_drand_address()){
                return recycle_drand(account_obj,account_meta);
            }
            return true;
        }
    
        bool  xvblockprune_impl::recycle_contract(const base::xvaccount_t & account_obj,base::xblockmeta_t & account_meta)
        {
            xinfo("xvblockprune_impl::recycle contract, %s, %llu, %llu, %llu", account_obj.get_address().c_str(), account_meta._highest_full_block_height, account_meta._lowest_vkey2_block_height, account_meta._highest_deleted_block_height);
            if (m_prune_contract.find(account_obj.get_address()) == m_prune_contract.end())
                return false;
            if (m_prune_contract[account_obj.get_address()] == enum_prune_none)
                return false;

            if(account_meta._highest_full_block_height <= enum_reserved_blocks_count) //start prune at least > 8
                return false;
             
            //[lower_bound_height,upper_bound_height)
            uint64_t upper_bound_height = account_meta._highest_full_block_height - enum_reserved_blocks_count;
            const uint64_t lower_bound_height = std::max(account_meta._lowest_vkey2_block_height,account_meta._highest_deleted_block_height) + 1;
            
            xinfo("xvblockprune_impl::recycle contract %s, upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_connect_block_height);
            
            if(lower_bound_height >= upper_bound_height)
                return false;
            else if (upper_bound_height - lower_bound_height <= (enum_min_batch_recycle_blocks_count << 1))
                return false;
            xdbg("xvblockprune_impl::recycle contract %s, adjust upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_cert_block_height);

            const std::string begin_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,lower_bound_height);
            const std::string end_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,upper_bound_height);
            XMETRICS_GAUGE(metrics::xmetrics_tag_t::prune_block_contract, upper_bound_height-lower_bound_height+1);
            if(get_xvdb()->delete_range(begin_delete_key, end_delete_key))//["begin_key", "end_key")
            {
                xinfo("xvblockprune_impl::recycle contract,succsssful for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
                account_meta._highest_deleted_block_height = upper_bound_height - 1;
            }
            else
            {
                xerror("xvblockprune_impl::recycle contract,failed for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
            }
            return true;
        }
        
        bool  xvblockprune_impl::recycle_table(const base::xvaccount_t & account_obj,base::xblockmeta_t & account_meta)
        {
            uint64_t reserve_num = XGET_CONFIG(prune_reserve_number);
            xinfo("xvblockprune_impl::recycle table, %s, %llu, %llu, %llu, %llu", account_obj.get_address().c_str(),
                account_meta._highest_full_block_height, account_meta._lowest_vkey2_block_height, account_meta._highest_deleted_block_height, reserve_num);
            if(account_meta._highest_full_block_height <= enum_reserved_blocks_count) //start prune at least > 8
                return false;
            uint64_t upper_bound_height = account_meta._highest_full_block_height - enum_reserved_blocks_count;
            const uint64_t lower_bound_height = std::max(account_meta._lowest_vkey2_block_height,account_meta._highest_deleted_block_height) + 1;
            
            xinfo("xvblockprune_impl::recycle table %s, upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_connect_block_height);
            
            if(lower_bound_height >= upper_bound_height)
                return false;

            uint64_t boundary;
            get_prune_boundary(account_obj, boundary);
            if (boundary < upper_bound_height) {
                upper_bound_height = boundary;
            }
            xdbg("xvblockprune_impl::recycle table %s, adjust upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_connect_block_height);

            if (lower_bound_height >= upper_bound_height) {
                return false;
            } else if((upper_bound_height - lower_bound_height) <= reserve_num)
                return false;//collect big range for each prune op as performance consideration
            upper_bound_height = upper_bound_height - reserve_num; //(enum_min_batch_recycle_blocks_count << 1);
            const std::string begin_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,lower_bound_height);
            const std::string end_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,upper_bound_height);
            XMETRICS_GAUGE(metrics::xmetrics_tag_t::prune_block_table, upper_bound_height-lower_bound_height+1);
            if(get_xvdb()->delete_range(begin_delete_key, end_delete_key))//["begin_key", "end_key")
            {
                xinfo("xvblockprune_impl::recycle table,succsssful for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);

                account_meta._highest_deleted_block_height = upper_bound_height - 1;
            }
            else
            {
                xerror("xvblockprune_impl::recycle table,failed for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
            }
            return true;
        }

        bool  xvblockprune_impl::recycle_timer(const base::xvaccount_t & account_obj,base::xblockmeta_t & account_meta)
        {
            if(account_meta._highest_cert_block_height <= (enum_min_batch_recycle_blocks_count << 1))
                return false;
            
            //[lower_bound_height,upper_bound_height)
            uint64_t upper_bound_height = account_meta._highest_cert_block_height - (enum_min_batch_recycle_blocks_count << 1);
            const uint64_t lower_bound_height = std::max(account_meta._lowest_vkey2_block_height,account_meta._highest_deleted_block_height) + 1;
            
            xinfo("xvblockprune_impl::recycle timer %s, upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_cert_block_height);
            
            if(lower_bound_height >= upper_bound_height)
                return false;
            else if (upper_bound_height - lower_bound_height <= (enum_min_batch_recycle_blocks_count << 1))
                return false;
            xdbg("xvblockprune_impl::recycle timer %s, adjust upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_cert_block_height);

            const std::string begin_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,lower_bound_height);
            const std::string end_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,upper_bound_height);
            XMETRICS_GAUGE(metrics::xmetrics_tag_t::prune_block_timer, upper_bound_height-lower_bound_height+1);
            if(get_xvdb()->delete_range(begin_delete_key, end_delete_key))//["begin_key", "end_key")
            {
                xinfo("xvblockprune_impl::recycle,succsssful for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
                account_meta._highest_deleted_block_height = upper_bound_height - 1;
            }
            else
            {
                xerror("xvblockprune_impl::recycle,failed for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
            }
            return true;
        }

        bool  xvblockprune_impl::recycle_drand(const base::xvaccount_t & account_obj,base::xblockmeta_t & account_meta)
        {
            if(account_meta._highest_commit_block_height <= (enum_min_batch_recycle_blocks_count << 1))
                return false;
            
            //[lower_bound_height,upper_bound_height)
            uint64_t upper_bound_height = account_meta._highest_commit_block_height - (enum_min_batch_recycle_blocks_count << 1);
            const uint64_t lower_bound_height = std::max(account_meta._lowest_vkey2_block_height,account_meta._highest_deleted_block_height) + 1;
            
            xinfo("xvblockprune_impl::recycle drand %s, upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_commit_block_height);
            
            uint64_t boundary;

            auto exist = get_prune_boundary(account_obj, boundary);
            if (exist && (boundary < upper_bound_height)) {
                upper_bound_height = boundary;
            }

            if(lower_bound_height >= upper_bound_height)
                return false;
            else if (upper_bound_height - lower_bound_height <= (enum_min_batch_recycle_blocks_count << 1))
                return false;

            xdbg("xvblockprune_impl::recycle drand %s, adjust upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_commit_block_height);

            const std::string begin_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,lower_bound_height);
            const std::string end_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,upper_bound_height);
            XMETRICS_GAUGE(metrics::xmetrics_tag_t::prune_block_drand, upper_bound_height-lower_bound_height+1);
            if(get_xvdb()->delete_range(begin_delete_key, end_delete_key))//["begin_key", "end_key")
            {
                xinfo("xvblockprune_impl::recycle,succsssful for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
                account_meta._highest_deleted_block_height = upper_bound_height - 1;
            }
            else
            {
                xerror("xvblockprune_impl::recycle,failed for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
            }
            return true;
        }
    
        bool  xvblockprune_impl::recycle_unit(const base::xvaccount_t & account_obj,base::xblockmeta_t & account_meta)
        {
            if(account_meta._highest_full_block_height <= enum_reserved_blocks_count) //start prune at least > 8
                return false;
             
            //[lower_bound_height,upper_bound_height)

            uint64_t upper_bound_height = account_meta._highest_full_block_height - enum_reserved_blocks_count;
            const uint64_t lower_bound_height = std::max(account_meta._lowest_vkey2_block_height,account_meta._highest_deleted_block_height) + 1;
            

            xinfo("xvblockprune_impl::recycle unit %s, upper %llu, lower %llu, connect_height %llu", account_obj.get_address().c_str(),
                upper_bound_height, lower_bound_height, account_meta._highest_connect_block_height);
            
            if(lower_bound_height >= upper_bound_height)
                return false;
            else if((upper_bound_height - lower_bound_height) <= enum_min_batch_recycle_blocks_count)
                return false;//collect big range for each prune op as performance consideration
            
            upper_bound_height = upper_bound_height - enum_min_batch_recycle_blocks_count;

            const std::string begin_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,lower_bound_height);
            const std::string end_delete_key = base::xvdbkey_t::create_prunable_block_height_key(account_obj,upper_bound_height);
            XMETRICS_GAUGE(metrics::xmetrics_tag_t::prune_block_unit, upper_bound_height-lower_bound_height+1);
            if(get_xvdb()->delete_range(begin_delete_key, end_delete_key))//["begin_key", "end_key")
            {
                xinfo("xvblockprune_impl::recycle,succsssful for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);   
                account_meta._highest_deleted_block_height = upper_bound_height - 1;
            }
            else
            {
                xerror("xvblockprune_impl::recycle,failed for account %s from %llu to %llu", account_obj.get_address().c_str(), lower_bound_height, upper_bound_height);
            }

            return true;
        }

        bool  xvblockprune_impl::refresh(const chainbase::enum_xmodule_type mod_id, const base::xvaccount_t & account_obj, const uint64_t permit_prune_upper_boundary) {
            if(account_obj.is_table_address()) {
                // if consensus zone
                auto zone_id = account_obj.get_zone_index();
                if ((zone_id == base::enum_chain_zone_zec_index) || (zone_id == base::enum_chain_zone_beacon_index)) {
                    //return false;
                }
            } else if (!account_obj.is_drand_address()) {
                return false;
            }

            xinfo("xvblockprune_impl::refresh, account:%s, module:%llx, height:%llu", account_obj.get_address().c_str(), mod_id, permit_prune_upper_boundary);
            
            //设置lock为1
            std::unique_lock<std::mutex> lock(m_lock);
            auto mod_prune_boundary = m_prune_boundary.find(account_obj.get_account());
            if (mod_prune_boundary != m_prune_boundary.end()) {
                if (mod_prune_boundary->second.find(mod_id) != mod_prune_boundary->second.end()) {
                    if (mod_prune_boundary->second[mod_id] >= permit_prune_upper_boundary) {
                        return true;
                    }
                } 
                mod_prune_boundary->second[mod_id] = permit_prune_upper_boundary;
            } else {
//                std::map<chainbase::enum_xmodule_type, uint64_t> prune_boundary;
//                prune_boundary[mod_id] = permit_prune_upper_boundary;
//                m_prune_boundary[account_obj.get_account()] = prune_boundary;
                xinfo("xvblockprune_impl::refresh, unwatched m_prune_boundary, %s, %llx, %llu", account_obj.get_address().c_str(), mod_id, permit_prune_upper_boundary);
            }
            
            return true;
        }

        bool  xvblockprune_impl::watch(const chainbase::enum_xmodule_type mod_id, const base::xvaccount_t & account_obj) {
            if(account_obj.is_table_address()) {
                // if consensus zone
                auto zone_id = account_obj.get_zone_index();
                if ((zone_id == base::enum_chain_zone_zec_index) || (zone_id == base::enum_chain_zone_beacon_index)) {
                    //return false;
                }
            } else if (!account_obj.is_drand_address()) {
                return false;
            }

            xdbg("xvblockprune_impl::watch, account:%s, module:%llx", account_obj.get_address().c_str(), mod_id);

            //设置lock为1
            std::unique_lock<std::mutex> lock(m_lock);  
            auto mod_prune_boundary = m_prune_boundary.find(account_obj.get_account());
            if (mod_prune_boundary == m_prune_boundary.end()) {
                std::map<chainbase::enum_xmodule_type, uint64_t> prune_boundary;
                prune_boundary[mod_id] = 0;
                m_prune_boundary[account_obj.get_account()] = prune_boundary;
            } else {
                if (mod_prune_boundary->second.find(mod_id) == mod_prune_boundary->second.end()) {
                    mod_prune_boundary->second[mod_id] = 0;
                }
            }
            return true;
        }

        bool  xvblockprune_impl::unwatch(const chainbase::enum_xmodule_type mod_id, const base::xvaccount_t & account_obj) { 
            if(account_obj.is_table_address()) {
                // if consensus zone
                auto zone_id = account_obj.get_zone_index();
                if ((zone_id == base::enum_chain_zone_zec_index) || (zone_id == base::enum_chain_zone_beacon_index)) {
                    //return false;
                }
            } else if (!account_obj.is_drand_address()) {
                return false;
            }

            xdbg("xvblockprune_impl::unwatch, account:%s, module:%llx", account_obj.get_address().c_str(), mod_id);

            //设置lock为1
            std::unique_lock<std::mutex> lock(m_lock);  
            auto mod_prune_boundary = m_prune_boundary.find(account_obj.get_account());
            if (mod_prune_boundary != m_prune_boundary.end()) {
                mod_prune_boundary->second.erase(mod_id);
                if (mod_prune_boundary->second.empty()) {
                    m_prune_boundary.erase(account_obj.get_account());
                }
            }

            return true;
        }

        bool xvblockprune_impl::get_prune_boundary(const base::xvaccount_t & account_obj, uint64_t &height) {
            //设置lock为1
            std::unique_lock<std::mutex> lock(m_lock); 
            auto mod_prune_boundary = m_prune_boundary.find(account_obj.get_account());
            if (mod_prune_boundary == m_prune_boundary.end()) {
                height = 0;
                return false;
            }
            
            uint64_t min_prune_boundary = (uint64_t)-1;
            for (auto it : mod_prune_boundary->second) {
                if (it.second < min_prune_boundary) {
                    min_prune_boundary = it.second;
                }
            }
            height = min_prune_boundary;
            return true;
        }
    }
}
