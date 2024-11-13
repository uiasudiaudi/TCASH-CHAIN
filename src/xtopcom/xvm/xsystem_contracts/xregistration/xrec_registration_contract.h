// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xrole_type.h"
#include "xdata/xsystem_contract/xdata_structures.h"
#include "xvm/xcontract/xcontract_base.h"
#include "xvm/xcontract/xcontract_exec.h"

#include <type_traits>

NS_BEG2(top, xstake)

using namespace xvm;
using namespace xvm::xcontract;

class xrec_registration_contract final: public xcontract_base {
    using xbase_t = xcontract_base;
public:
    XDECLARE_DELETED_COPY_DEFAULTED_MOVE_SEMANTICS(xrec_registration_contract);
    XDECLARE_DEFAULTED_OVERRIDE_DESTRUCTOR(xrec_registration_contract);

    explicit
    xrec_registration_contract(common::xnetwork_id_t const & network_id);

    xcontract_base*
    clone() override {return new xrec_registration_contract(network_id());}

    /**
     * @brief setup the contract
     *
     */
    void setup();

    /**
     * @brief register the node
     *
     * @param node_types
     * @param nickname
     * @param signing_key
     */
    void registerNode(const std::string & node_types,
                      const std::string & nickname,
                      const std::string & signing_key,
                      const uint32_t dividend_rate
#if defined XENABLE_MOCK_ZEC_STAKE
                   , common::xaccount_address_t const & registration_account
#endif
    );

    /**
     * @brief unregister the node
     *
     */
    void unregisterNode();

    void updateNodeInfo(const std::string & nickname, const int updateDepositType, const uint64_t deposit, const uint32_t dividend_rate, const std::string & node_types, const std::string & node_sign_key);
    /**
     * @brief Set the Dividend Ratio
     *
     * @param dividend_rate
     */
    void setDividendRatio(uint32_t dividend_rate);

    /**
     * @brief Set the Node Name
     *
     * @param nickname
     */
    void setNodeName(const std::string & nickname);

    // void update_node_credit(std::set<std::string> const& accounts);

    /**
     * @brief batch update stakes
     *
     * @param adv_votes
     */
    void update_batch_stake(std::map<std::string, std::string> const& adv_votes);

    /**
     * @brief batch update stakes v2, considering trx splitting
     *
     * @param report_time
     * @param contract_adv_votes
     */
    void update_batch_stake_v2(uint64_t report_time, std::map<std::string, std::string> const & contract_adv_votes);

    /**
     * @brief redeem node deposit
     *
     */
    void redeemNodeDeposit();

    /**
     * @brief update node type
     *
     * @param node_types
     */
    void updateNodeType(const std::string & node_types);

    /**
     * @brief stake deposit
     *
     */
    void stakeDeposit();

    /**
     * @brief unstake deposit
     *
     * @param unstake_deposit
     */
    void unstakeDeposit(uint64_t unstake_deposit);

    void updateNodeSignKey(const std::string & node_sign_key);

    /**
     * @brief slave unqualified node
     *
     * @param punish_node_str
     */
    void slash_unqualified_node(std::string const& punish_node_str);

    BEGIN_CONTRACT_WITH_PARAM(xrec_registration_contract)
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, registerNode);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, unregisterNode);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, updateNodeInfo);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, setDividendRatio);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, setNodeName);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, update_batch_stake);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, update_batch_stake_v2);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, redeemNodeDeposit);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, updateNodeType);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, stakeDeposit);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, unstakeDeposit);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, updateNodeSignKey);
        CONTRACT_FUNCTION_PARAM(xrec_registration_contract, slash_unqualified_node);
    END_CONTRACT_WITH_PARAM

private:
    /**
     * @brief register the node
     *
     * @param node_types
     * @param nickname
     * @param signing_key
     * @param network_ids
     */
    void registerNode2(const std::string & node_types,
                       const std::string & nickname,
                       const std::string & signing_key,
                       const uint32_t dividend_rate,
                       const std::set<common::xnetwork_id_t> & network_ids
#if defined XENABLE_MOCK_ZEC_STAKE
                     , common::xaccount_address_t const & registration_account
#endif
    );

    /**
     * @brief update node info
     *
     * @param node_info
     */
    void update_node_info(data::system_contract::xreg_node_info const & node_info);

    /**
     * @brief delete node info
     *
     * @param account
     */
    void        delete_node_info(std::string const & account);

    /**
     * @brief Get the node info
     *
     * @param account
     * @param node_info
     * @return int32_t
     */
    int32_t get_node_info(const std::string & account, data::system_contract::xreg_node_info & node_info);

    /**
     * @brief insert the refund info
     *
     * @param account
     * @param refund_amount
     * @return int32_t
     */
    int32_t     ins_refund(const std::string& account, uint64_t const & refund_amount);

    /**
     * @brief delete the refund info
     *
     * @param account
     * @return int32_t
     */
    int32_t     del_refund(const std::string& account);

    /**
     * @brief Get the refund info
     *
     * @param account
     * @param refund
     * @return int32_t
     */
    int32_t get_refund(const std::string & account, data::system_contract::xrefund_info & refund);

    /**
     * @brief check and set mainnet activation
     *
     */
    void        check_and_set_genesis_stage();

    /**
     * @brief Get the slash info
     *
     * @param account
     * @param node_slash_info
     * @return int32_t
     */
    int32_t get_slash_info(std::string const & account, data::system_contract::xslash_info & node_slash_info);
    /**
     * @brief get slash staking time
     *
     * @param node_addr
     */
    void        slash_staking_time(std::string const& node_addr);

    /**
     * @brief check if a valid nickname
     *
     * @param nickname
     * @return true
     * @return false
     */
    bool        is_valid_name(const std::string & nickname) const;

    /**
     * @brief check if signing key exists
     *
     * @param signing_key
     * @return true
     * @return false
     */
    bool        check_if_signing_key_exist(const std::string & signing_key);

    /**
     * @brief
     *
     * @param report_time
     * @param last_report_time
     * @param contract_adv_votes
     * @param merge_contract_adv_votes
     * @return true
     * @return false
     */
    bool        handle_receive_shard_votes(uint64_t report_time, uint64_t last_report_time, std::map<std::string, std::string> const & contract_adv_votes, std::map<std::string, std::string> & merge_contract_adv_votes);


    /**
     * @brief init validator/auditor node credit
     *
     * @param node_info
     */
    void init_node_credit(data::system_contract::xreg_node_info & node_info);
};


NS_END2
