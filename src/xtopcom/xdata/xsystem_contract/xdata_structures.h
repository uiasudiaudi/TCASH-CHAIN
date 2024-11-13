// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xmem.h"
#include "xbase/xutl.h"
#include "xbasic/xcrypto_key.h"
#include "xbasic/xserializable_based_on.h"
#include "xbasic/xserialize_face.h"
#include "xbasic/xuint.hpp"
#include "xcommon/xaddress.h"
#include "xcommon/xlogic_time.h"
#include "xcommon/xrole_type.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdata/xdata_common.h"
#include "xcommon/common.h"
#include "xdata/xsystem_contract/xallowance.h"

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <ctime>
#include <set>
#include <string>
#include <vector>

NS_BEG3(tcash, data, system_contract)

XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_REG_KEY = "@101";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_TIME_KEY = "@102";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_WORKLOAD_KEY = "@103";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_SHARD_KEY = "@104";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_TICKETS_KEY = "@105";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_POLLABLE_KEY = "@107";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_TASK_KEY = "@111";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTES_KEY_BASE = "@112";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTES_KEY1 = "@112-1";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTES_KEY2 = "@112-2";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTES_KEY3 = "@112-3";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTES_KEY4 = "@112-4";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_INEFFECTIVE_VOTES_KEY = "@113";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_MAX_TIME_KEY = "@115";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_AWARD_KEY = "@118";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VALIDATOR_KEY = "@120";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTER_DIVIDEND_REWARD_KEY_BASE = "@121";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTER_DIVIDEND_REWARD_KEY1 = "@121-1";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTER_DIVIDEND_REWARD_KEY2 = "@121-2";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTER_DIVIDEND_REWARD_KEY3 = "@121-3";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTER_DIVIDEND_REWARD_KEY4 = "@121-4";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_NODE_REWARD_KEY = "@124";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VALIDATOR_WORKLOAD_KEY = "@125";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_TABLEBLOCK_HEIGHT_KEY = "@126";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_START_HEIGHT_KEY = "@127";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_REFUND_KEY = "@128";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_GENESIS_STAGE_KEY = "@129";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_TGAS_KEY = "@130";

// slash related
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_UNQUALIFIED_NODE_KEY = "@131";
XINLINE_CONSTEXPR char const * XPROPERTY_CONTRACT_SLASH_INFO_KEY = "@132";
XINLINE_CONSTEXPR char const * XPROPERTY_CONTRACT_TABLEBLOCK_NUM_KEY = "@133";
XINLINE_CONSTEXPR char const * XPROPERTY_CONTRACT_EXTENDED_FUNCTION_KEY = "@134";

// reward
XINLINE_CONSTEXPR char const * XPROPERTY_CONTRACT_ACCUMULATED_ISSUANCE = "@141";
XINLINE_CONSTEXPR char const * XPROPERTY_CONTRACT_ACCUMULATED_ISSUANCE_YEARLY = "@142";
XINLINE_CONSTEXPR char const * XPROPERTY_LAST_READ_REC_REG_CONTRACT_BLOCK_HEIGHT = "@143";
XINLINE_CONSTEXPR char const * XPROPERTY_LAST_READ_REC_REG_CONTRACT_LOGIC_TIME = "@144";
XINLINE_CONSTEXPR char const * XPORPERTY_CONTRACT_VOTE_REPORT_TIME_KEY = "@145";
XINLINE_CONSTEXPR char const * XPROPERTY_REWARD_DETAIL = "@146";

// crosschain
XINLINE_CONSTEXPR char const * XPROPERTY_LAST_HASH = "@160";
XINLINE_CONSTEXPR char const * XPROPERTY_EFFECTIVE_HASHES = "@161";
XINLINE_CONSTEXPR char const * XPROPERTY_ALL_HASHES = "@162";
XINLINE_CONSTEXPR char const * XPROPERTY_HEADERS = "@163";
XINLINE_CONSTEXPR char const * XPROPERTY_HEADERS_SUMMARY = "@164";
XINLINE_CONSTEXPR char const * XPROPERTY_RESET_FLAG = "@165";

XINLINE_CONSTEXPR char const * XPROPERTY_RELAY_ELECT_PACK_HEIGHT = "@170";
XINLINE_CONSTEXPR char const * XPROPERTY_RELAY_WRAP_PHASE = "@171";
XINLINE_CONSTEXPR char const * XPROPERTY_RELAY_BLOCK_STR = "@172";

XINLINE_CONSTEXPR char const * XPROPERTY_NODE_CHECK_OPTION_KEY = "@173";
XINLINE_CONSTEXPR char const * XPROPERTY_NODE_ROOT_ACCOUNT_KEY = "@174";
XINLINE_CONSTEXPR char const * XPROPERTY_NODE_ROOT_CA_KEY = "@175";
XINLINE_CONSTEXPR char const * XPROPERTY_NODE_INFO_MAP_KEY = "@176";

// eth2
XINLINE_CONSTEXPR char const * XPROPERTY_FINALIZED_EXECUTION_BLOCKS = "@180";
XINLINE_CONSTEXPR char const * XPROPERTY_UNFINALIZED_HEADERS = "@181";
XINLINE_CONSTEXPR char const * XPROPERTY_FINALIZED_BEACON_HEADER = "@182";
XINLINE_CONSTEXPR char const * XPROPERTY_FINALIZED_EXECUTION_HEADER = "@183";
XINLINE_CONSTEXPR char const * XPROPERTY_CURRENT_SYNC_COMMITTEE = "@184";
XINLINE_CONSTEXPR char const * XPROPERTY_NEXT_SYNC_COMMITTEE = "@185";
XINLINE_CONSTEXPR char const * XPROPERTY_CLIENT_MODE = "@186";
XINLINE_CONSTEXPR char const * XPROPERTY_UNFINALIZED_HEAD_EXECUTION_HEADER = "@187";
XINLINE_CONSTEXPR char const * XPROPERTY_UNFINALIZED_TAIL_EXECUTION_HEADER = "@188";

constexpr char const * XTRANSFER_ACTION{"transfer"};
constexpr char const * XZEC_WORKLOAD_CLEAR_WORKLOAD_ACTION{"clear_workload"};
constexpr char const * XREWARD_CLAIMING_ADD_NODE_REWARD{"recv_node_reward"};
constexpr char const * XREWARD_CLAIMING_ADD_VOTER_DIVIDEND_REWARD{"recv_voter_dividend_reward"};

int const XPROPERTY_SPLITED_NUM = 4;
int const DENOMINATOR = 10000;
uint64_t const REWARD_PRECISION = 1000000;

#ifdef PERIOD_MOCK
constexpr common::xlogic_time_t REDEEM_INTERVAL = 2;  // 72 hours
#else
constexpr common::xlogic_time_t REDEEM_INTERVAL = 25920;  // 72 hours
#endif

// percent * 10^2 * 10^6,  total_issue_base / 10^2 / 10^6
constexpr uint32_t INITIAL_YEAR_PERCENT = 3040000;
constexpr uint32_t FINAL_YEAR_PERCENT = 2000000;
constexpr uint64_t TOTAL_ISSUE_BASE = TOTAL_ISSUANCE / 100 / 1e6;

constexpr uint64_t TOTAL_RESERVE = TOTAL_ISSUANCE * 38 / 100;
constexpr uint64_t TIMER_BLOCK_HEIGHT_PER_YEAR = 3155815;

/**
 * @brief         check if mainnet can be activated
 *
 * @param nodes   node registration property map
 * @return true
 * @return false
 */
bool check_registered_nodes_active(std::map<std::string, std::string> const & nodes);

struct xreward_node_record final : xserializable_based_on<void> {
    // common::xminer_type_t m_registered_miner_type {common::xminer_type_t::invalid};
    ::uint128_t m_accumulated{0};
    ::uint128_t m_unclaimed{0};
    uint64_t m_last_claim_time{0};
    uint64_t m_issue_time{0};

private:
    /**
     * @brief           write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief           read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct node_record_t final : public xserializable_based_on<void> {
    std::string account;
    ::uint128_t accumulated{0};
    ::uint128_t unclaimed{0};
    common::xlogic_time_t last_claim_time{0};
    uint64_t issue_time{0};

private:
    /**
     * @brief           write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief           read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xreward_record final : public xserializable_based_on<void> {
    ::uint128_t accumulated{0};
    ::uint128_t unclaimed{0};
    std::vector<node_record_t> node_rewards;
    common::xlogic_time_t last_claim_time{0};
    uint64_t issue_time{0};

private:
    /**
     * @brief           write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief           read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xaccumulated_reward_record final : public xserializable_based_on<void> {
    common::xlogic_time_t last_issuance_time{0};
    ::uint128_t issued_until_last_year_end{0};

private:
    /**
     * @brief           write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief           read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xrefund_info final : public xserializable_based_on<void> {
    uint64_t refund_amount{0};
    common::xlogic_time_t create_time{0};

private:
    /**
     * @brief           write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief           read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xnode_vote_percent_t final : public xserializable_based_on<void> {
    uint32_t block_count;
    uint32_t subset_count;

private:
    std::int32_t do_write(base::xstream_t & stream) const override;

    std::int32_t do_read(base::xstream_t & stream) override;
};

struct xunqualified_node_info_v1_t final : public xserializable_based_on<void> {
    std::map<common::xnode_id_t, xnode_vote_percent_t> auditor_info;
    std::map<common::xnode_id_t, xnode_vote_percent_t> validator_info;

private:
    int32_t do_write(base::xstream_t & stream) const override;

    int32_t do_read(base::xstream_t & stream) override;
};

struct xunqualified_filter_info_t final : public xserializable_based_on<void> {
    common::xnode_id_t node_id;
    common::xnode_type_t node_type;
    uint32_t vote_percent;

private:
    int32_t do_write(base::xstream_t & stream) const override;

    int32_t do_read(base::xstream_t & stream) override;
};

struct xaction_node_info_t final : public xserializable_based_on<void> {
    common::xnode_id_t node_id;
    common::xnode_type_t node_type;
    bool action_type;  // default true for punish
    xaction_node_info_t() : node_id(common::xnode_id_t{}), node_type(common::xnode_type_t::invalid), action_type(true) {}
    xaction_node_info_t(common::xnode_id_t _node_id, common::xnode_type_t _node_type, bool type = true) : node_id(_node_id), node_type(_node_type), action_type(type) {}

private:
    int32_t do_write(base::xstream_t & stream) const override;

    int32_t do_read(base::xstream_t & stream) override;
};

struct account_stake_t final {
    account_stake_t(account_stake_t const &) = default;
    account_stake_t & operator=(account_stake_t const &) = default;
    account_stake_t(account_stake_t &&) = default;
    account_stake_t & operator=(account_stake_t &&) = default;
    ~account_stake_t() = default;

    /**
     * @brief Construct a new account stake t object
     *
     * @param in_account
     * @param in_stake
     */
    account_stake_t(std::string const & in_account, uint64_t in_stake) : account{in_account}, stake{in_stake} {
    }

    /**
     * @brief less than other's stake
     *
     * @param other
     * @return true
     * @return false
     */
    bool operator<(account_stake_t const & other) const noexcept {
        if (stake != other.stake) {
            return stake < other.stake;
        }

        return account < other.account;
    }

    /**
     * @brief greater than other's stake
     *
     * @param other
     * @return true
     * @return false
     */
    bool operator>(account_stake_t const & other) const noexcept {
        return other < *this;
    }

    std::string account;
    uint64_t stake;
};

template <common::xminer_type_t MinerTypeV>
uint64_t minimal_deposit_of();

template <>
uint64_t minimal_deposit_of<common::xminer_type_t::edge>();

template <>
uint64_t minimal_deposit_of<common::xminer_type_t::archive>();

template <>
uint64_t minimal_deposit_of<common::xminer_type_t::exchange>();

template <>
uint64_t minimal_deposit_of<common::xminer_type_t::advance>();

template <>
uint64_t minimal_deposit_of<common::xminer_type_t::validator>();

template <common::xnode_type_t NodeTypeV>
bool could_be(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::rec>(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::zec>(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::consensus_auditor>(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::consensus_validator>(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::storage_archive>(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::storage_exchange>(common::xminer_type_t const miner_type);

template <>
bool could_be<common::xnode_type_t::edge>(common::xminer_type_t const miner_type);

struct xreg_node_info final : public xserializable_based_on<void> {
private:
    common::xminer_type_t m_registered_miner_type{common::xminer_type_t::invalid};
    bool m_genesis{false};

public:
    xreg_node_info() = default;
    xreg_node_info(xreg_node_info const &) = default;
    xreg_node_info & operator=(xreg_node_info const &) = default;
    xreg_node_info(xreg_node_info &&) = default;
    xreg_node_info & operator=(xreg_node_info &&) = default;
    ~xreg_node_info() override = default;

    /// @brief Check to see if this node could be an rec based on miner type.
    bool could_be_rec() const noexcept;

    /// @brief Check to see if this node could be a zec based on miner type.
    bool could_be_zec() const noexcept;

    /// @brief Check to see if this node could be an auditor based on miner type.
    bool could_be_auditor() const noexcept;

    /// @brief Check to see if this node could be a validator based on miner type.
    bool could_be_validator() const noexcept;

    /// @brief Check to see if this node could be an archive based on miner type.
    bool could_be_archive() const noexcept;

    /// @brief Check to see if this node could be an archive based on minter type.
    bool legacy_could_be_archive() const noexcept;

    /// @brief Check to see if this node could be an edge based on miner type.
    bool could_be_edge() const noexcept;

    /// @brief Check to see if this node could be an exchange node based on miner type.
    bool could_be_exchange() const noexcept;

    /// @brief Check to see if this account could be a fullnode node based on miner type.
    bool could_be_fullnode() const noexcept;

    /// @brief Check to see if this account could be a fullnode node based on miner type.
    bool could_be_fullnode_legacy() const noexcept;

    /// @brief Check to see if this account coule be an evm auditor node based on miner type.
    bool could_be_evm_auditor() const noexcept;

    /// @brief Check to see if this account coule be an evm validator node based on miner type.
    bool could_be_evm_validator() const noexcept;

    /// @brief Check to see if this account could be an relay node based on miner type.
    bool could_be_relay() const noexcept;

    /// @brief Check to see if this node can be an rec based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_rec() const noexcept;

    /// @brief Check to see if this node can be a zec based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_zec() const noexcept;

    /// @brief Check to see if this node can be a validator based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_validator() const noexcept;

    /// @brief Check to see if this node can be an auditor based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_auditor() const noexcept;

    /// @brief Check to see if this node can be an archive based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_archive() const noexcept;

    /// @brief Check to see if this node can be an edge based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_edge() const noexcept;

    /// @brief Check to see if this node can be an exchange based on miner type and other information (e.g. deposit, amount of received tickets).
    bool can_be_exchange() const noexcept;

    /// @brief Check to see if this account can be a fullnode based on miner type and other information (e.g. deposit, amount of received tickects).
    bool can_be_fullnode() const noexcept;

    /// @brief Check to see if this account can be a fullnode based on miner type and other information (e.g. deposit, amount of received tickects).
    bool can_be_fullnode_legacy() const noexcept;

    /// @brief Check to see if this account can be an eth based on miner type and other information (e.g. deposit, amount of received tickects).
    bool can_be_evm_auditor() const noexcept;

    /// @brief Check to see if this account can be an eth based on miner type and other information (e.g. deposit, amount of received tickects).
    bool can_be_evm_validator() const noexcept;

    /// @brief Check to see if this account can be a relayer based on miner type and other information (e.g. deposit, amount of received tickects).
    bool can_be_relay() const noexcept;

    template <common::xminer_type_t MinerTypeV>
    bool has() const noexcept {
        xdbg("queried miner type: %s, registered miner type %s", common::to_string(MinerTypeV).c_str(), common::to_string(m_registered_miner_type).c_str());
        return common::has<MinerTypeV>(m_registered_miner_type);
    }

    /**
     * @brief check if self is an invlid node
     *
     * @return true
     * @return false
     */
    bool is_invalid_node() const noexcept {
        return m_registered_miner_type == common::xminer_type_t::invalid;
    }

    bool genesis() const noexcept;

    void genesis(bool v) noexcept;

    uint64_t deposit() const noexcept;

    bool has_enough_tickets() const noexcept;

    /**
     * @brief get rec stake
     *
     * @return uint64_t
     */
    uint64_t rec_stake() const noexcept;
    /**
     * @brief get zec stake
     *
     * @return uint64_t
     */
    uint64_t zec_stake() const noexcept;
    /**
     * @brief get auditor stake
     *
     * @return uint64_t
     */
    uint64_t auditor_stake() const noexcept;
    /**
     * @brief get validator stake
     *
     * @return uint64_t
     */
    uint64_t validator_stake() const noexcept;
    /**
     * @brief get edge stake
     *
     * @return uint64_t
     */
    uint64_t edge_stake() const noexcept;
    /**
     * @brief get archive stake
     *
     * @return uint64_t
     */
    uint64_t archive_stake() const noexcept;

    uint64_t exchange_stake() const noexcept;

    uint64_t fullnode_stake() const noexcept;

    uint64_t evm_auditor_stake() const noexcept;

    uint64_t evm_validator_stake() const noexcept;

    uint64_t relay_stake() const noexcept;

    /// @brief Get miner type.
    common::xminer_type_t miner_type() const noexcept;

    void miner_type(common::xminer_type_t new_miner_type) noexcept;

    /**
     * @brief Get auditor stake
     *
     * @return uint64_t
     */
    uint64_t get_auditor_stake() const noexcept {
        uint64_t stake = 0;
        if (could_be_auditor()) {
            stake = (m_account_mortgage / tcash_UNIT + m_vote_amount / 2) * m_auditor_credit_numerator / m_auditor_credit_denominator;
        }
        return stake;
    }

    /**
     * @brief Get validator stake
     *
     * @return uint64_t
     */
    uint64_t get_validator_stake() const noexcept {
        uint64_t stake = 0;
        if (could_be_validator()) {
            auto max_validator_stake = XGET_ONCHAIN_GOVERNANCE_PARAMETER(max_validator_stake);
            stake = (uint64_t)sqrt((m_account_mortgage / tcash_UNIT + m_vote_amount / 2) * m_validator_credit_numerator / m_validator_credit_denominator);
            stake = stake < max_validator_stake ? stake : max_validator_stake;
        }
        return stake;
    }

    template <common::xminer_type_t MinerTypeV>
    bool miner_type_has() const noexcept {
        return common::has<MinerTypeV>(miner_type());
    }

    uint64_t raw_credit_score_data(common::xnode_type_t const node_type) const noexcept;

    /**
     * @brief Get required min deposit
     *
     * @return uint64_t
     */
    uint64_t get_required_min_deposit() const noexcept;

    /**
     * @brief deduce credit score
     *
     * @param node_type
     */
    void slash_credit_score(common::xnode_type_t node_type);
    /**
     * @brief increase credit score
     *
     * @param node_type
     */
    void award_credit_score(common::xnode_type_t node_type);

    common::xaccount_address_t m_account{};
    uint64_t m_account_mortgage{0};
    uint64_t m_vote_amount{0};
    uint64_t m_auditor_credit_numerator{0};
    uint64_t m_auditor_credit_denominator{1000000};
    uint64_t m_validator_credit_numerator{0};
    uint64_t m_validator_credit_denominator{1000000};

    uint m_support_ratio_numerator{0};  // dividends to voters
    uint m_support_ratio_denominator{100};
    // uint64_t    m_stake {0};
    // stake_info  m_stake_info {};
    common::xlogic_time_t m_last_update_time{0};
    // uint32_t    m_network_id {0};
    std::set<common::xnetwork_id_t> m_network_ids;
    std::string nickname;
    xpublic_key_t consensus_public_key;

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xslash_info final : public xserializable_based_on<void> {
public:
    common::xlogic_time_t m_punish_time{0};
    common::xlogic_time_t m_staking_lock_time{0};
    uint32_t m_punish_staking{0};

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xgroup_workload_t final : public xserializable_based_on<void> {
    std::string group_address_str;
    uint32_t group_total_workload{0};
    std::map<std::string, uint32_t> m_leader_count;

    xgroup_workload_t & operator+=(xgroup_workload_t const & adder);

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return std::int32_t
     */
    std::int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief read from stream
     *
     * @param stream
     * @return std::int32_t
     */
    std::int32_t do_read(base::xstream_t & stream) override;
};

struct xgroup_cons_reward_t final : public xserializable_based_on<void> {
    std::string group_address_str;
    std::map<std::string, uint64_t> m_leader_reward;

    xgroup_cons_reward_t & operator+=(xgroup_cons_reward_t const & adder);

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return std::int32_t
     */
    std::int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief read from stream
     *
     * @param stream
     * @return std::int32_t
     */
    std::int32_t do_read(base::xstream_t & stream) override;
};

struct xactivation_record final : public xserializable_based_on<void> {
    int activated{0};
    common::xlogic_time_t activation_time{0};

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};
struct xreward_dispatch_task final : public xserializable_based_on<void> {
    uint64_t onchain_timer_round;
    std::string contract;
    std::string action;
    std::string params;

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_write(base::xstream_t & stream) const override {
        int32_t const begin_pos = stream.size();
        stream << onchain_timer_round;
        stream << contract;
        stream << action;
        stream << params;
        int32_t const end_pos = stream.size();
        return (end_pos - begin_pos);
    }

    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override {
        int32_t const begin_pos = stream.size();
        stream >> onchain_timer_round;
        stream >> contract;
        stream >> action;
        stream >> params;
        int32_t const end_pos = stream.size();
        return (begin_pos - end_pos);
    }
};

struct reward_detail_v1 final : public xserializable_based_on<void> {
    ::uint128_t m_edge_reward{0};
    ::uint128_t m_archive_reward{0};
    ::uint128_t m_validator_reward{0};
    ::uint128_t m_auditor_reward{0};
    ::uint128_t m_vote_reward{0};
    ::uint128_t m_self_reward{0};

private:
    int32_t do_write(base::xstream_t & stream) const override;

    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct reward_detail_v2 final : public xserializable_based_on<void> {
    ::uint128_t m_edge_reward{0};
    ::uint128_t m_archive_reward{0};
    ::uint128_t m_validator_reward{0};
    ::uint128_t m_auditor_reward{0};
    ::uint128_t m_evm_validator_reward{0};
    ::uint128_t m_evm_auditor_reward{0};
    ::uint128_t m_vote_reward{0};
    ::uint128_t m_self_reward{0};

    explicit operator reward_detail_v1() const;

private:
    int32_t do_write(base::xstream_t & stream) const override;

    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

class xissue_detail_v1 final : public xserializable_based_on<void> {
public:
    uint64_t onchain_timer_round{0};
    uint64_t m_zec_vote_contract_height{0};
    uint64_t m_zec_workload_contract_height{0};
    uint64_t m_zec_reward_contract_height{0};
    uint16_t m_edge_reward_ratio{0};
    uint16_t m_archive_reward_ratio{0};
    uint16_t m_validator_reward_ratio{0};
    uint16_t m_auditor_reward_ratio{0};
    uint16_t m_vote_reward_ratio{0};
    uint16_t m_governance_reward_ratio{0};
    uint64_t m_auditor_group_count{0};
    uint64_t m_validator_group_count{0};
    std::map<std::string, reward_detail_v1> m_node_rewards;

public:
    std::string to_string() const;
    int32_t from_string(std::string const & s);

private:
    int32_t do_write(base::xstream_t & stream) const override;

    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

class xissue_detail_v2 final : public xserializable_based_on<void> {
public:
    uint64_t onchain_timer_round{0};
    uint64_t m_zec_vote_contract_height{0};
    uint64_t m_zec_workload_contract_height{0};
    uint64_t m_zec_reward_contract_height{0};
    uint16_t m_edge_reward_ratio{0};
    uint16_t m_archive_reward_ratio{0};
    uint16_t m_validator_reward_ratio{0};
    uint16_t m_auditor_reward_ratio{0};
    uint16_t m_evm_auditor_reward_ratio{0};
    uint16_t m_evm_validator_reward_ratio{0};
    uint16_t m_vote_reward_ratio{0};
    uint16_t m_governance_reward_ratio{0};
    uint64_t m_auditor_group_count{0};
    uint64_t m_validator_group_count{0};
    uint64_t m_evm_auditor_group_count{0};
    uint64_t m_evm_validator_group_count{0};
    std::map<std::string, reward_detail_v2> m_node_rewards;

public:
    explicit operator xissue_detail_v1() const;
    std::string to_string() const;
    int32_t from_string(std::string const & s);

private:
    int32_t do_write(base::xstream_t & stream) const override;

    /**
     * @brief read from stream
     *
     * @param stream
     * @return int32_t
     */
    int32_t do_read(base::xstream_t & stream) override;
};

struct xnode_manage_account_info_t final : public xserializable_based_on<void> {
    uint64_t        reg_time;
    uint64_t        expiry_time;
    uint64_t        cert_time;
    std::string     account;
    std::string     cert_info;

private:
    /**
     * @brief write to stream
     *
     * @param stream
     * @return std::int32_t
     */
    std::int32_t do_write(base::xstream_t & stream) const override;
    /**
     * @brief read from stream
     *
     * @param stream
     * @return std::int32_t
     */
    std::int32_t do_read(base::xstream_t & stream) override;
};

// class xtcash_allowance {
// public:
//     using data_type = std::map<common::xaccount_address_t, evm_common::u256>;
//     using allocator_type = data_type::allocator_type;
//     using const_iterator = data_type::const_iterator;
//     using const_pointer = data_type::const_pointer;
//     using const_reference = data_type::const_reference;
//     using difference_type = data_type::difference_type;
//     using iterator = data_type::iterator;
//     using key_type = data_type::key_type;
//     using mapped_type = data_type::mapped_type;
//     using pointer = data_type::pointer;
//     using reference = data_type::reference;
//     using size_type = data_type::size_type;
//     using value_type = data_type::value_type;

//     xtcash_allowance() = default;
//     xtcash_allowance(xtcash_allowance const &) = delete;
//     xtcash_allowance & operator=(xtcash_allowance const &) = delete;
//     xtcash_allowance(xtcash_allowance &&) = default;
//     xtcash_allowance & operator=(xtcash_allowance &&) = default;
//     ~xtcash_allowance() = default;

//     explicit xtcash_allowance(data_type d) noexcept(std::is_nothrow_move_constructible<data_type>::value);

//     // iterators
//     iterator begin() noexcept;
//     const_iterator begin() const noexcept;
//     const_iterator cbegin() const noexcept;

//     iterator end() noexcept;
//     const_iterator end() const noexcept;
//     const_iterator cend() const noexcept;

//     // capacity
//     bool empty() const noexcept;
//     size_type size() const noexcept;

//     // modifier
//     std::pair<iterator, bool> insert(const value_type & value);
//     iterator insert(const_iterator hint, const value_type & value);

//     // lookup
//     size_type count(key_type const & key) const;
//     iterator find(key_type const & key);
//     const_iterator find(key_type const & key) const;

//     data_type const & raw_data() const noexcept;

// private:
//     data_type data_;
// };
// using xallowance_t = xtcash_allowance;

NS_END3
