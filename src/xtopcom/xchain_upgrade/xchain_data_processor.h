// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xlegacy_account_address.h"

namespace tcash
{
    namespace chain_data
    {
        struct data_processor_t {
            std::string address;
            int64_t tcash_balance{0};
            int64_t burn_balance{0};
            int64_t tgas_balance{0};
            int64_t vote_balance{0};
            int64_t lock_balance{0};
            int64_t lock_tgas{0};
            int64_t unvote_num{0};
            int64_t expire_vote{0};
            int64_t create_time{0};
            int64_t lock_token{0};
            std::vector<std::string> pledge_vote;
        };
        class xtcash_chain_data_processor
        {
        public:
            static bool check_state();
            static bool set_state();

            static void get_user_data(common::xlegacy_account_address_t const &addr, data_processor_t & reset_data);
            static void get_all_user_data(std::vector<data_processor_t> & reset_data);
            static std::map<common::xaccount_address_t, data_processor_t> get_all_user_data();

            static void get_contract_data(common::xlegacy_account_address_t const & addr, data_processor_t & reset_data);
            static void get_all_contract_data(std::vector<data_processor_t> & reset_data);
            static void get_stake_string_property(common::xlegacy_account_address_t const & addr, std::string const & property, std::string & value);
            static void get_stake_map_property(common::xlegacy_account_address_t const & addr,
                                               std::string const & property,
                                               std::vector<std::pair<std::string, std::string>> & map);
            
            static void release();
        };
        using xchain_data_processor_t = xtcash_chain_data_processor;
    }
}
