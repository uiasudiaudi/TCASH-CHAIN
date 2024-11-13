// Copyright (c) 2018-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xfixed_hash.h"
#include "xbasic/xmemory.hpp"
#include "xevm_common/trie/xtrie_db_fwd.h"
#include "xevm_common/trie/xtrie_node_fwd.h"
#include "xevm_common/trie/xtrie_pruner_fwd.h"

#include <unordered_set>

NS_BEG3(tcash, evm_common, trie)

class xtcash_trie_pruner {
    std::unordered_set<xh256_t> trie_node_hashes_;

public:
    xtcash_trie_pruner() = default;
    xtcash_trie_pruner(xtcash_trie_pruner const &) = delete;
    xtcash_trie_pruner & operator=(xtcash_trie_pruner const &) = delete;
    xtcash_trie_pruner(xtcash_trie_pruner &&) = default;
    xtcash_trie_pruner & operator=(xtcash_trie_pruner &&) = default;
    ~xtcash_trie_pruner() = default;

    void init(std::shared_ptr<xtrie_node_face_t> const & trie_root, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);

    void prune(xh256_t const & old_trie_root_hash, observer_ptr<xtrie_db_t> trie_db, std::unordered_set<xh256_t> & pruned_hashes, std::error_code & ec);

private:
    std::shared_ptr<xtrie_node_face_t> load_trie_node(std::shared_ptr<xtrie_node_face_t> const & trie_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    std::shared_ptr<xtrie_short_node_t> load_short_node(std::shared_ptr<xtrie_short_node_t> const & short_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    std::shared_ptr<xtrie_full_node_t> load_full_node(std::shared_ptr<xtrie_full_node_t> const & full_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    std::shared_ptr<xtrie_node_face_t> load_hash_node(std::shared_ptr<xtrie_hash_node_t> const & hash_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    void load_full_node_children(std::shared_ptr<xtrie_full_node_t> const & full_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);

    void try_prune_trie_node(std::shared_ptr<xtrie_node_face_t> const & trie_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    void try_prune_hash_node(std::shared_ptr<xtrie_hash_node_t> const & hash_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    void try_prune_short_node(std::shared_ptr<xtrie_short_node_t> const & short_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);
    void try_prune_full_node(std::shared_ptr<xtrie_full_node_t> const & full_node, observer_ptr<xtrie_db_t> trie_db, std::error_code & ec);

    void try_prune_trie_node(std::shared_ptr<xtrie_node_face_t> const & trie_node,
                             observer_ptr<xtrie_db_t> trie_db,
                             std::unordered_set<xh256_t> & pruned_hashes,
                             std::error_code & ec);
    void try_prune_hash_node(std::shared_ptr<xtrie_hash_node_t> const & hash_node,
                             observer_ptr<xtrie_db_t> trie_db,
                             std::unordered_set<xh256_t> & pruned_hashes,
                             std::error_code & ec);
    void try_prune_short_node(std::shared_ptr<xtrie_short_node_t> const & short_node,
                              observer_ptr<xtrie_db_t> trie_db,
                              std::unordered_set<xh256_t> & pruned_hashes,
                              std::error_code & ec);
    void try_prune_full_node(std::shared_ptr<xtrie_full_node_t> const & full_node,
                             observer_ptr<xtrie_db_t> trie_db,
                             std::unordered_set<xh256_t> & pruned_hashes,
                             std::error_code & ec);
};
using xtrie_pruner_t = xtcash_trie_pruner;

NS_END3
