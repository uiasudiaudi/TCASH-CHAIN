// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "xdata/xverifier/xtx_verifier.h"

#include "xbase/xutl.h"
#include "xbasic/xmodule_type.h"
#include "xchain_fork/xutility.h"
#include "xdata/xgenesis_data.h"
#include "xdata/xnative_contract_address.h"
#include "xdata/xsystem_contract/xdata_structures.h"
#include "xdata/xverifier/xverifier_utl.h"
#include "xdata/xverifier/xwhitelist_verifier.h"
#include "xdata/xverifier/xblacklist_verifier.h"
#include "xvledger/xvblock.h"
#include "xstatestore/xstatestore_face.h"
#include "xconfig/xconfig_register.h"
#include "xpbase/base/tcash_utils.h"
#include "xdata/xrootblock.h"
#include <cinttypes>

NS_BEG2(tcash, xverifier)

REG_XMODULE_LOG(chainbase::enum_xmodule_type::xmodule_type_xverifier, xverifier_error_to_string, xverifier_error_base+1, xverifier_error_max);

int32_t xtx_verifier::verify_address(data::xtransaction_t const * trx) {
    const auto & src_addr = trx->source_address();
    const auto & dst_addr = trx->target_address();

    if (src_addr.empty() || dst_addr.empty()) {
        xwarn("[global_trace][xtx_verifier][address_verify][fail], tx:%s", trx->dump().c_str());
        return xverifier_error::xverifier_error_addr_invalid;
    }

    if (xverifier_error::xverifier_success != xverifier::xtx_utl::address_is_valid(src_addr.to_string(), true) ||
        xverifier_error::xverifier_success != xverifier::xtx_utl::address_is_valid(dst_addr.to_string(), true)) {
        xwarn("[global_trace][xtx_verifier][address_verify][address invalid], tx:%s,%s,%s", trx->dump().c_str(), src_addr.to_string().c_str(), dst_addr.to_string().c_str());
        return  xverifier_error::xverifier_error_addr_invalid;
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_burn_tx(data::xtransaction_t const * trx) {
    const auto & src_addr = trx->source_address();
    const auto & dst_addr = trx->target_address();
    bool const is_src_black_hole_addr = data::is_black_hole_address(src_addr);
    bool const is_dst_black_hole_addr = data::is_black_hole_address(dst_addr);

    if ( (!is_src_black_hole_addr) && (!is_dst_black_hole_addr) ) {
        // not burn tx
        return xverifier_error::xverifier_success;
    }
    // src addr should be user addr; must be transfer type;
    if ( ( (!data::is_account_address(common::xaccount_address_t{src_addr})) /* && (!data::is_sub_account_address(common::xaccount_address_t{src_addr}))*/ )
        || (!is_dst_black_hole_addr)
        || (trx->get_tx_type() != data::enum_xtransaction_type::xtransaction_type_transfer) ) {
        xwarn("[global_trace][xtx_verifier][verify_burn_tx] fail, tx:%s", trx->dump().c_str());
        return xverifier_error_burn_tx_invalid;
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_local_tx(data::xtransaction_t const * trx) {
    const auto & src_addr = trx->source_address();
    const auto & dst_addr = trx->target_address();
    if (!common::is_t2_address(src_addr)) {
        // not local tx
        return xverifier_error::xverifier_success;
    }

    if ( (src_addr != dst_addr)
        || (trx->get_tx_type() != data::enum_xtransaction_type::xtransaction_type_run_contract)
        || (!trx->get_authorization().empty()) ) {
        xwarn("[global_trace][xtx_verifier][verify_local_tx][fail], tx:%s,invalid local tx", trx->dump().c_str());
        return xverifier_error_local_tx_invalid;
    }

    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_address_type(data::xtransaction_t const * trx) {
    const auto & src_addr = trx->source_address();
    const auto & dst_addr = trx->target_address();
    base::enum_vaccount_addr_type const src_addr_type = src_addr.type();
    base::enum_vaccount_addr_type const dst_addr_type = dst_addr.type();

    // source addr only can be T0,T8,T2
    if ( (src_addr_type != base::enum_vaccount_addr_type_secp256k1_user_account)
        && (src_addr_type != base::enum_vaccount_addr_type_secp256k1_eth_user_account)
        && (src_addr_type != base::enum_vaccount_addr_type_secp256k1_evm_user_account)
        && (src_addr_type != base::enum_vaccount_addr_type_native_contract) ) {
        xwarn("[global_trace][xtx_verifier][address_verify]src addr invalid, tx:%s", trx->dump().c_str());
        return  xverifier_error::xverifier_error_addr_invalid;
    }

    // target addr only can be T0,T8,T2,blackhole
    if ( (dst_addr_type != base::enum_vaccount_addr_type_secp256k1_user_account)
        && (dst_addr_type != base::enum_vaccount_addr_type_native_contract)
        && (dst_addr_type != base::enum_vaccount_addr_type_secp256k1_eth_user_account)
        && (dst_addr_type != base::enum_vaccount_addr_type_secp256k1_evm_user_account)
        && (dst_addr_type != base::enum_vaccount_addr_type_black_hole) ) {
        xwarn("[global_trace][xtx_verifier][address_verify]dst addr invalid, tx:%s", trx->dump().c_str());
        return  xverifier_error::xverifier_error_addr_invalid;
    }

    if (trx->get_tx_type() == data::enum_xtransaction_type::xtransaction_type_transfer) {
        if ( (src_addr_type != base::enum_vaccount_addr_type_secp256k1_user_account)
            && (src_addr_type != base::enum_vaccount_addr_type_secp256k1_eth_user_account)
            && (src_addr_type != base::enum_vaccount_addr_type_secp256k1_evm_user_account) ) {
            xwarn("[global_trace][xtx_verifier][address_verify]src addr invalid , tx:%s", trx->dump().c_str());
            return  xverifier_error::xverifier_error_addr_invalid;
        }
        if ( (dst_addr_type != base::enum_vaccount_addr_type_secp256k1_user_account)
            && (dst_addr_type != base::enum_vaccount_addr_type_secp256k1_eth_user_account)
            && (dst_addr_type != base::enum_vaccount_addr_type_secp256k1_evm_user_account)
            && (dst_addr_type != base::enum_vaccount_addr_type_black_hole) ) {
            xwarn("[global_trace][xtx_verifier][address_verify]dst addr invalid , tx:%s", trx->dump().c_str());
            return  xverifier_error::xverifier_error_addr_invalid;
        }
        if (!trx->is_tcash_transfer() && dst_addr_type == base::enum_vaccount_addr_type_black_hole) {
            xwarn("[global_trace][xtx_verifier][address_verify]dst addr invalid , tx:%s", trx->dump().c_str());
            return xverifier_error::xverifier_error_addr_invalid;
        }

        if(verify_transaction_whitelist(src_addr.to_string(), dst_addr.to_string()) != xverifier_error::xverifier_success) {
           xwarn("[global_trace][xtx_verifier][address_verify]verify_transaction_whitelist addr invalid , tx:%s", trx->dump().c_str());
           return xverifier_error::xverifier_error_addr_invalid;
        }
    }

    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_tx_signature(data::xtransaction_t const * trx) {
    // verify signature
    if (common::is_t2_address(trx->source_address())) {
        xdbg("[global_trace][xtx_verifier][verify_tx_signature][success], tx:%s", trx->dump().c_str());
        return xverifier_error::xverifier_success;
    }

    if (trx->target_address() == rec_standby_pool_contract_address) {
        xdbg("[global_trace][xtx_verifier][verify_tx_signature][success], tx:%s", trx->dump().c_str());
        return xverifier_error::xverifier_success;
    }

    xdbg("[global_trace][xtx_verifier][verify_tx_signature][sign_check], tx:%s", trx->dump().c_str());
    bool const check_success = trx->sign_check();
    if (!check_success) {
        xwarn("[global_trace][xtx_verifier][signature_verify][fail], tx:%s", trx->dump().c_str());
        return xverifier_error::xverifier_error_tx_signature_invalid;
    }

    xdbg("[global_trace][xtx_verifier][verify_tx_signature][success], tx:%s", trx->dump().c_str());
    return xverifier_error::xverifier_success;
}

// verify trx fire expiration
int32_t xtx_verifier::verify_tx_fire_expiration(data::xtransaction_t const * trx, uint64_t now, bool is_first_time_push_tx) {
    uint32_t trx_fire_tolerance_time = XGET_ONCHAIN_GOVERNANCE_PARAMETER(tx_send_timestamp_tolerance);

    if (trx->get_fire_timestamp() == 0)
    {
        xdbg("[global_trace][xtx_verifier][verify_tx_fire_expiration][success], tx:%s", trx->dump().c_str());
        return xverifier_error::xverifier_success;
    }

    uint64_t fire_expire;
    if (is_first_time_push_tx) {
        // XTODO only first time to txpool should check if fire timestamp is too old
        fire_expire = trx->get_fire_timestamp() + trx_fire_tolerance_time;
        if (fire_expire < now) {
            xwarn("xtx_verifier::verify_tx_fire_expiration fail fire timetamp too old, tx:%s, fire_timestamp:%ld, now:%ld",
                trx->dump().c_str(), trx->get_fire_timestamp(), now);
            return xverifier_error::xverifier_error_tx_fire_expired;
        }
    }

    fire_expire = trx->get_fire_timestamp() + trx->get_expire_duration() + trx_fire_tolerance_time;
    if (fire_expire < now) {
        xwarn("xtx_verifier::verify_tx_fire_expiration fail expired, tx:%s, fire_timestamp:%" PRIu64 ", fire_tolerance_time:%" PRIu32 ", expire_duration:%" PRIu16 ", now:%" PRIu64,
            trx->dump().c_str(), trx->get_fire_timestamp(), trx_fire_tolerance_time, trx->get_expire_duration(), now);
        return xverifier_error::xverifier_error_tx_duration_expired;
    }

    fire_expire = now + trx_fire_tolerance_time;
    if (fire_expire < trx->get_fire_timestamp()) {
        xwarn("xtx_verifier::verify_tx_fire_expiration fail fire timetamp too future, tx:%s, fire_timestamp:%ld, now:%ld",
            trx->dump().c_str(), trx->get_fire_timestamp(), now);
        return xverifier_error::xverifier_error_tx_fire_expired;
    }

    xdbg("[global_trace][xtx_verifier][verify_tx_fire_expiration][success], tx:%s", trx->dump().c_str());
    return xverifier_error::xverifier_success;

}

int32_t xtx_verifier::sys_contract_tx_check(data::xtransaction_t const * trx_ptr) {
    static std::vector<common::xaccount_address_t> open_sys_contracts = {
        rec_registration_contract_address,
        rec_standby_pool_contract_address,
        table_vote_contract_address,
        rec_tcc_contract_address,
        table_reward_claiming_contract_address,
#if defined(XBUILD_CONSORTIUM)
        rec_node_manage_address
#endif
    };

    if (trx_ptr->get_target_action_name() == "nodeJoinNetwork") {
        return xverifier_error::xverifier_error_contract_not_allowed;
    }

    auto const & source_addr = trx_ptr->source_address();
    auto const & target_addr = trx_ptr->target_address_unadjusted();

    auto const & sender_addr = source_addr;
    auto const & recver_addr = target_addr;
    if (common::is_t6_address(sender_addr) && common::is_t2_address(recver_addr)) {
        xinfo("T6 account is not allowed to call system contract");
        return xverifier_error::xverifier_error_t6_not_allowed_to_call_contract;
    }

    bool const source_is_user_addr            = data::is_account_address(sender_addr); // || data::is_sub_account_address(sender_addr);
    bool const target_is_sys_contract_addr    = common::is_t2_address(recver_addr);

    if(verify_node_whitelist(source_addr.to_string(), target_addr.to_string()) != xverifier_error::xverifier_success) {
        xdbg("[global_trace][xtx_verifier][verify_node_whitelist][failed], tx:%s", trx_ptr->dump().c_str());
        return xverifier_error::xverifier_error_addr_invalid;
    }

    if(verify_transaction_whitelist(source_addr.to_string(), target_addr.to_string()) != xverifier_error::xverifier_success) {
        xdbg("[global_trace][xtx_verifier][verify_transaction_whitelist][failed], tx:%s", trx_ptr->dump().c_str());
        return xverifier_error::xverifier_error_addr_invalid;
    }

    if (source_is_user_addr && target_is_sys_contract_addr) {
        for (const auto & addr : open_sys_contracts) {
            if (addr == target_addr) {
                xdbg("[global_trace][xtx_verifier][sys_contract_tx_check][success], tx:%s", trx_ptr->dump().c_str());
                return xverifier_error::xverifier_success;
            }
        }
        xwarn("[global_trace][xtx_verifier][sys_contract_tx_check][fail], tx:%s,target_origin_addr=%s", trx_ptr->dump().c_str(), target_addr.to_string().c_str());
        return xverifier_error::xverifier_error_contract_not_allowed;
    }

    xdbg("[global_trace][xtx_verifier][sys_contract_tx_check][success], tx:%s", trx_ptr->dump().c_str());
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_account_min_deposit(uint64_t amount) {
    auto account_min_deposit = XGET_CONFIG(min_account_deposit);
    if (amount < account_min_deposit) {
        xwarn("[global_trace][xtx_verifier][verify_account_min_deposit][fail], trx min deposit:%d, amount:%ld", account_min_deposit, amount);
        return xverifier_error_account_min_deposit_invalid;
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_tx_min_deposit(uint64_t deposit) {
     auto trx_min_deposit = XGET_ONCHAIN_GOVERNANCE_PARAMETER(min_tx_deposit);
    if (deposit < trx_min_deposit) {
        xwarn("[global_trace][xtx_verifier][verify_tx_min_deposit][fail], trx min deposit:%d, tx deposit:%ld", trx_min_deposit, deposit);
        return xverifier_error_tx_min_deposit_invalid;
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_send_tx_source(data::xtransaction_t const * trx_ptr, bool local) {
    base::enum_vaccount_addr_type const addr_type = trx_ptr->source_address().type();
    if (local) {
        if (addr_type != base::enum_vaccount_addr_type_native_contract) {
            xwarn("[global_trace][xtx_verifier][verify_send_tx_source][fail], tx:%s,local tx not sys contract tx", trx_ptr->dump().c_str());
            return xverifier_error_send_tx_source_invalid;
        }
        int32_t ret = verify_local_tx(trx_ptr);
        if (ret) {
            return ret;
        }
    } else {
        bool valid_addr_type = (addr_type == base::enum_vaccount_addr_type_secp256k1_user_account)
                                // || (addr_type == base::enum_vaccount_addr_type_secp256k1_user_sub_account)
                                || (addr_type == base::enum_vaccount_addr_type_secp256k1_evm_user_account)
                                || (addr_type == base::enum_vaccount_addr_type_secp256k1_eth_user_account);
        if (!valid_addr_type) {
            xwarn("[global_trace][xtx_verifier][verify_send_tx_source][fail], tx:%s,non_local tx addr type invalid", trx_ptr->dump().c_str());
            return xverifier_error_send_tx_source_invalid;
        }
        if (trx_ptr->get_authorization().empty()) {
            xwarn("[global_trace][xtx_verifier][verify_send_tx_source][fail], tx:%s,non_local tx must has signature", trx_ptr->dump().c_str());
            return xverifier_error_send_tx_source_invalid;
        }
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_send_tx_validation(data::xtransaction_t const * trx_ptr) {
    if (!trx_ptr->transaction_type_check()) {
        xwarn("[global_trace][xtx_verifier][verify_send_tx_validation][fail], tx:%s,tx type invalid", trx_ptr->dump().c_str());
        return xverifier_error_tx_basic_validation_invalid;
    }
    if (!trx_ptr->unuse_member_check()) {
        xwarn("[global_trace][xtx_verifier][verify_send_tx_validation][fail], tx:%s,unuse member check invalid", trx_ptr->dump().c_str());
        return xverifier_error_tx_basic_validation_invalid;
    }
    if (!trx_ptr->transaction_len_check()) {
        xwarn("[global_trace][xtx_verifier][verify_send_tx_validation][fail], tx:%s,len check invalid", trx_ptr->dump().c_str());
        return xverifier_error::xverifier_error_tx_param_invalid;
    }
    int32_t ret = verify_address_type(trx_ptr);
    if (ret) {
        return ret;
    }
    ret = verify_address(trx_ptr);
    if (ret) {
        return ret;
    }
    ret = verify_burn_tx(trx_ptr);
    if (ret) {
        return ret;
    }
    ret = verify_local_tx(trx_ptr);
    if (ret) {
        return ret;
    }
    ret = sys_contract_tx_check(trx_ptr);
    if (ret) {
        return ret;
    }
    // verify hash
    if (!trx_ptr->digest_check()) {
        xwarn("xtx_verifier::verify_send_tx_validation, tx:%s digest check invalid", trx_ptr->dump().c_str());
        return xverifier_error::xverifier_error_tx_hash_invalid;
    }
    if (xverifier::xblacklist_utl_t::is_black_address(trx_ptr->source_address().to_string(), trx_ptr->target_address().to_string())) {
        xwarn("[xtx_verifier::verify_send_tx_validation] in black address,tx:%s", trx_ptr->dump().c_str());
        return xverifier_error::xverifier_error_tx_blacklist_invalid;
    }

    if (xverifier::xwhitelist_utl::is_white_address_limit(trx_ptr->source_address().to_string())) {
        xwarn("[xtx_verifier::verify_send_tx_validation] whitelist limit address,tx:%s", trx_ptr->dump().c_str());
        return xverifier_error::xverifier_error_tx_whitelist_invalid;
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_send_tx_legitimacy(data::xtransaction_t const * trx_ptr) {
    int32_t ret = verify_tx_signature(trx_ptr);
    if (ret) {
        return ret;
    }
    ret = verify_shard_contract_addr(trx_ptr);
    if (ret) {
        return ret;
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_shard_contract_addr(data::xtransaction_t const * trx_ptr) {
    const auto & source_addr = trx_ptr->source_address();
    const auto & origin_target_addr = trx_ptr->target_address_unadjusted();
    // user call sys sharding contract, always auto set target addresss
    if (data::is_sys_sharding_contract_address(origin_target_addr)) {
        if (data::is_account_address(source_addr)) {
            if (std::string::npos != origin_target_addr.to_string().find("@")) {
                xwarn("[global_trace][xtx_verifier][verify_shard_contract_addr] fail-already set tableid, tx:%s,origin_target_addr=%s", trx_ptr->dump().c_str(), origin_target_addr.to_string().c_str());
                return xverifier_error_tx_basic_validation_invalid;
            }

            const auto & target_addr = trx_ptr->target_address();
            if (std::string::npos == target_addr.to_string().find("@")) {
                xwarn("[global_trace][xtx_verifier][verify_shard_contract_addr] fail-not set tableid, tx:%s,target_addr=%s", trx_ptr->dump().c_str(), target_addr.to_string().c_str());
                return xverifier_error_tx_basic_validation_invalid;
            }

            base::xvaccount_t _src_vaddr(source_addr.to_string());
            base::xvaccount_t _dst_vaddr(target_addr.to_string());
            if (_src_vaddr.get_ledger_subaddr() != _dst_vaddr.get_ledger_subaddr()
                || _src_vaddr.get_ledger_id() != _dst_vaddr.get_ledger_id()) {
                xwarn("[global_trace][xtx_verifier][verify_shard_contract_addr] fail-src and dst not match, tx:%s,target_addr=%s", trx_ptr->dump().c_str(), target_addr.to_string().c_str());
                return xverifier_error_tx_basic_validation_invalid;
            }
        } else {
            if (std::string::npos == origin_target_addr.to_string().find("@")) {
                xwarn("[global_trace][xtx_verifier][verify_shard_contract_addr] fail-sys contract all should set full addr, tx:%s,origin_target_addr=%s", trx_ptr->dump().c_str(), origin_target_addr.to_string().c_str());
                return xverifier_error_tx_basic_validation_invalid;
            }
        }
    }
    return xverifier_error::xverifier_success;
}

int32_t xtx_verifier::verify_node_whitelist(const std::string& source_addr, const std::string& target_addr)
{

#if defined(XBUILD_CONSORTIUM)
    // consortium: check register whitelist
    if (XGET_ONCHAIN_GOVERNANCE_PARAMETER(enable_node_whitelist) == true) {
        auto const sender_addr = common::xaccount_address_t { source_addr };
        bool source_is_user_addr = data::is_account_address(sender_addr);

        if (source_is_user_addr && target_addr == sys_contract_rec_registration_addr) {
            std::string nodes = XGET_ONCHAIN_GOVERNANCE_PARAMETER(node_whitelist);
            std::set<std::string> node_sets;
            tcash::SplitString(nodes, ',', node_sets);

            if (node_sets.find(source_addr) != node_sets.end()) {
                return xverifier_error::xverifier_success;
            }

            xwarn("xtx_verifier::verify_node_whitelist fail, source_addr %s target_addr %s", source_addr.c_str(), target_addr.c_str());
            return xverifier_error::xverifier_error_whitelist_limit;
        }
    }
#endif
    return xverifier_error::xverifier_success;
}

bool xtx_verifier::verify_check_genesis_account(const std::string& account)
{
    std::map<std::string, uint64_t> genesis_accounts = data::xrootblock_t::get_all_genesis_accounts();
    auto it = genesis_accounts.find(account);
    if (it != genesis_accounts.end()) {
        return true;
    }

    return false;
}

 // consortium: check transfer address
int32_t xtx_verifier::verify_transaction_whitelist(const std::string& src_addr, const std::string& dst_addr) {
#if defined(XBUILD_CONSORTIUM)
    if (XGET_ONCHAIN_GOVERNANCE_PARAMETER(enable_transaction_whitelist) == true) {
        
        if(verify_check_genesis_account(src_addr) ||  verify_check_genesis_account(dst_addr)) {
            return xverifier_error::xverifier_success;
        }
        
        auto const sender_addr = common::xaccount_address_t{src_addr};
        if (data::is_sys_contract_address(sender_addr)) {
            return xverifier_error::xverifier_success;
        }

        std::string nodes = XGET_ONCHAIN_GOVERNANCE_PARAMETER(transaction_whitelist);
        std::set<std::string> node_sets;
        tcash::SplitString(nodes, ',', node_sets);
        if (node_sets.find(src_addr) == node_sets.end() && node_sets.find(dst_addr) == node_sets.end() ) {
            xwarn("[global_trace][xtx_verifier][address_verify] check whitelist address fail, src_addr:%s dst_addr:%s.", src_addr.c_str(), dst_addr.c_str());
            return xverifier_error::xverifier_error_addr_invalid;
        }
    }
#endif
    return xverifier_error::xverifier_success;
}

NS_END2
