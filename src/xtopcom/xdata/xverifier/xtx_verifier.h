// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include "xdata/xtransaction.h"
#include "xbasic/xmemory.hpp"
#include "xdata/xverifier/xblacklist_verifier.h"
#include "xdata/xverifier/xwhitelist_verifier.h"

NS_BEG2(tcash, xverifier)

class xtx_verifier {
public:
    /**
     * @brief  verify sigature, hash, content(address/balance)
     *
     * @param trx  the transaction to verify
     * @param store  the store object to get info
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_tx_signature(data::xtransaction_t const * trx);

    /**
     * @brief verify address whether valid
     *
     * @param trx  the transaction to verify
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_address(data::xtransaction_t const * trx);

    /**
     * @brief verify address according type
     *
     * @param trx  the transaction to verify
     * @param tx_subtype  the transaction subtype
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_address_type(data::xtransaction_t const * trx);
    /**
     * @brief verify trx fire expiration
     *
     * @param trx  the transaction to verify
     * @param now  current time
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_tx_fire_expiration(data::xtransaction_t const * trx, uint64_t now, bool is_first_time_push_tx);

    /**
     * @brief verify trx fired by user to allowed system contracts
     *
     * @param trx_ptr   the transction to verify
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t sys_contract_tx_check(data::xtransaction_t const * trx_ptr);

    /**
     * @brief verify account min deposit
     *
     * @param amount  the amount to verify
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_account_min_deposit(uint64_t amount);

    /**
     * @brief verify tx min deposit
     *
     * @param amount  the amount to verify
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_tx_min_deposit(uint64_t amount);

    /**
     * @brief verify send tx souce. the send tx may come from local or network.
     * local tx must be a self system contract tx and should not has signature
     * non local tx must not be a system contract tx and must has valid signature
     *
     * @param trx_ptr  the transaction to verify
     * @param local  the transaction comes from local or network
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_send_tx_source(data::xtransaction_t const * trx_ptr, bool local);

    /**
     * @brief verify send tx validation.
     *
     * @param trx_ptr  the transaction to verify
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_send_tx_validation(data::xtransaction_t const * trx_ptr);
    /**
     * @brief verify send tx legitimacy, which includes signature and whitelist check
     *
     * @param trx_ptr  the transaction to verify
     * @return int32_t  see xverifier_errors definition
     */
    static int32_t verify_send_tx_legitimacy(data::xtransaction_t const * trx_ptr);

    /**
     * @brief check account if genesis account
     * 
     * @param account 
     * @return true 
     * @return false 
     */
    static bool  verify_check_genesis_account(const std::string& account);
    
 private:
    static int32_t verify_burn_tx(data::xtransaction_t const * trx);
    static int32_t verify_local_tx(data::xtransaction_t const * trx);
    static int32_t verify_shard_contract_addr(data::xtransaction_t const * trx_ptr);
    static int32_t    verify_node_whitelist(const std::string& source_addr, const std::string& target_addr);
    static int32_t    verify_transaction_whitelist(const std::string& src_addr, const std::string& dst_addr);
};


NS_END2
