// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xmemory.hpp"
#include "xcommon/xtable_address.h"
#include "xevm_common/trie/xtrie_db.h"
#include "xvledger/xvdbstore.h"

NS_BEG2(tcash, state_mpt)

class xtcash_state_mpt_caching_db {
public:
    xtcash_state_mpt_caching_db(xtcash_state_mpt_caching_db const &) = delete;
    xtcash_state_mpt_caching_db & operator=(xtcash_state_mpt_caching_db const &) = delete;
    xtcash_state_mpt_caching_db(xtcash_state_mpt_caching_db &&) = default;
    xtcash_state_mpt_caching_db & operator=(xtcash_state_mpt_caching_db &&) = default;
    ~xtcash_state_mpt_caching_db() = default;

    explicit xtcash_state_mpt_caching_db(base::xvdbstore_t * db);

    observer_ptr<evm_common::trie::xtrie_db_t> trie_db(common::xtable_address_t const & table) const;

private:
    std::map<common::xtable_address_t, std::shared_ptr<evm_common::trie::xtrie_db_t>> table_caches_;
};
using xstate_mpt_caching_db_t = xtcash_state_mpt_caching_db;

NS_END2
