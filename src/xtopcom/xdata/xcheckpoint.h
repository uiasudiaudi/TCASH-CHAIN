// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xobject_ptr.h"
#include "xcommon/xaddress.h"
#include "xvledger/xvstate.h"

namespace tcash {
namespace data {

struct xtcash_checkpoint_data {
    uint64_t height{0};
    std::string hash;
};
using xcheckpoint_data_t = xtcash_checkpoint_data;

using xcheckpoints_t = std::map<uint64_t, xcheckpoint_data_t>;
using xcheckpoints_map_t = std::map<common::xaccount_address_t, xcheckpoints_t>;

class xtcash_chain_checkpoint {
public:
    /// @brief Load all cp data and state into memory.
    static void load();

    /// @brief Get latest cp data of specific table.
    /// @param account Table account only.
    /// @param ec Error code.
    /// @return Cp data which include hash and clock.
    static xcheckpoint_data_t get_latest_checkpoint(common::xaccount_address_t const & account, std::error_code & ec);

    /// @brief Get all cp data of specific table.
    /// @param account Table account only.
    /// @param ec Error code.
    /// @return Cp data map with all heights: <clock, {block_height, block_hash...}>.
    static xcheckpoints_t get_checkpoints(common::xaccount_address_t const & account, std::error_code & ec);

private:
    static xcheckpoints_map_t m_checkpoints_map;
};
using xchain_checkpoint_t = xtcash_chain_checkpoint;

}  // namespace data
}  // namespace tcash
