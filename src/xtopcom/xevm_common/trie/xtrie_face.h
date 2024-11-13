// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xfixed_hash.h"
#include "xevm_common/trie/xtrie_db.h"

NS_BEG3(tcash, evm_common, trie)

class xtcash_trie_face {
public:
    xtcash_trie_face() = default;
    xtcash_trie_face(xtcash_trie_face const &) = delete;
    xtcash_trie_face & operator=(xtcash_trie_face const &) = delete;
    xtcash_trie_face(xtcash_trie_face &&) = default;
    xtcash_trie_face & operator=(xtcash_trie_face &&) = default;
    virtual ~xtcash_trie_face() = default;

    // Hash returns the root hash of the trie. It does not write to the database and
    // can be used even if the trie doesn't have one.
    virtual xh256_t hash() = 0;

    // TryGet returns the value for key stored in the trie. The value bytes must
    // not be modified by the caller. If a node was not found in the database, a
    // trie.MissingNodeError is returned.
    virtual xbytes_t try_get(xbytes_t const & key, std::error_code & ec) const = 0;

    // TryUpdate associates key with value in the trie. If value has length zero, any
    // existing value is deleted from the trie. The value bytes must not be modified
    // by the caller while they are stored in the trie. If a node was not found in the
    // database, a trie.MissingNodeError is returned.
    virtual void try_update(xbytes_t const & key, xbytes_t const & value, std::error_code & ec) = 0;

    // TryDelete removes any existing value for key from the trie. If a node was not
    // found in the database, a trie.MissingNodeError is returned.
    virtual void try_delete(xbytes_t const & key, std::error_code & ec) = 0;

    // Commit writes all nodes to the trie's memory database, tracking the internal
    // and external (for account tries) references.
    virtual std::pair<xh256_t, int32_t> commit(std::error_code & ec) = 0;

    // Prove constructs a Merkle proof for key. The result contains all encoded nodes
    // on the path to the value at key. The value itself is also included in the last
    // node and can be retrieved by verifying the proof.
    //
    // If the trie does not contain a value for key, the returned proof contains all
    // nodes of the longest existing prefix of the key (at least the root), ending
    // with the node that proves the absence of the key.
    virtual bool prove(xbytes_t const & key, uint32_t from_level, xkv_db_face_ptr_t proof_db, std::error_code & ec) = 0;

    virtual void prune(xh256_t const & old_trie_root_hash, std::unordered_set<xh256_t> & pruned_hashes, std::error_code & ec) = 0;
    virtual void commit_pruned(std::unordered_set<xh256_t> const & pruned_hashes, std::error_code & ec) = 0;

    virtual void prune(std::error_code & ec) = 0;
    virtual void commit_pruned(std::vector<xh256_t> pruned_root_hashes, std::error_code & ec) = 0;
    virtual void clear_pruned(xh256_t const & pending_pruned_trie_root_hash, std::error_code & ec) = 0;
    virtual void clear_pruned(std::error_code & ec) = 0;

    virtual xh256_t const & original_root_hash() const noexcept = 0;
};
using xtrie_face_t = xtcash_trie_face;
using xtrie_face_ptr_t = std::shared_ptr<xtrie_face_t>;

NS_END3
