// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xfixed_hash.h"
#include "xbasic/xlru_cache_specialize.h"
#include "xbasic/xthreading/xdummy_mutex.h"
#include "xevm_common/trie/xtrie_db_fwd.h"
#include "xevm_common/trie/xtrie_kv_db_face.h"
#include "xevm_common/trie/xtrie_node_fwd.h"

#include <functional>
#include <map>
#include <unordered_set>

NS_BEG3(tcash, evm_common, trie)

// fwd:
class xtcash_trie_cache_node;
using xtrie_cache_node_t = xtcash_trie_cache_node;

class xtcash_trie_db {
private:
    friend class xtcash_trie_cache_node;
    xkv_db_face_ptr_t diskdb_;  // Persistent storage for matured trie nodes

    basic::xlru_cache_t<xh256_t, xbytes_t, threading::xdummy_mutex_t> cleans_;
    std::map<xh256_t, xtrie_cache_node_t> dirties_;
    std::unordered_set<xh256_t> pruned_hashes_;
    std::unordered_map<xh256_t, std::vector<xh256_t>> pruned_hashes2_;

    // xh256_t oldest_;
    // xh256_t newest_;

    std::map<xh256_t, xbytes_t> preimages_;  // Preimages of nodes from the secure trie

    mutable std::mutex mutex_;

public:
    xtcash_trie_db(xtcash_trie_db const &) = delete;
    xtcash_trie_db & operator=(xtcash_trie_db const &) = delete;
    xtcash_trie_db(xtcash_trie_db &&) = delete;
    xtcash_trie_db & operator=(xtcash_trie_db &&) = delete;
    ~xtcash_trie_db() = default;

    explicit xtcash_trie_db(xkv_db_face_ptr_t diskdb, size_t cache_size);

    // NewDatabase creates a new trie database to store ephemeral trie content before
    // its written out to disk or garbage collected. No read cache is created, so all
    // data retrievals will hit the underlying disk database.
    static std::shared_ptr<xtcash_trie_db> NewDatabase(xkv_db_face_ptr_t diskdb, size_t cache_size = 5000);

    // NewDatabaseWithConfig creates a new trie database to store ephemeral trie content
    // before its written out to disk or garbage collected. It also acts as a read cache
    // for nodes loaded from disk.
    // static std::shared_ptr<xtcash_trie_db> NewDatabaseWithConfig(xkv_db_face_ptr_t diskdb, xtrie_db_config_ptr_t config, size_t cache_size);

    xkv_db_face_ptr_t const & DiskDB() const noexcept;

    // insert inserts a collapsed trie node into the memory database.
    // The blob size must be specified to allow proper size tracking.
    // All nodes inserted by this function will be reference tracked
    // and in theory should only used for **trie nodes** insertion.
    void insert(xh256_t const & hash, int32_t size, xtrie_node_face_ptr_t const & node);

    // insertPreimage writes a new trie node pre-image to the memory database if it's
    // yet unknown. The method will NOT make a copy of the slice,
    // only use if the preimage will NOT be changed later on.
    //
    // Note, this method assumes that the database's lock is held!
    void insertPreimage(xh256_t const & hash, xbytes_t const & preimage);

    // node retrieves a cached trie node from memory, or returns nil if none can be
    // found in the memory cache.
    xtrie_node_face_ptr_t node(xh256_t const & hash);

    // Node retrieves an encoded cached trie node from memory. If it cannot be found
    // cached, the method queries the persistent database for the content.
    xbytes_t Node(xh256_t const & hash, std::error_code & ec);

    xbytes_t preimage(xh256_t const & hash) const;

    using AfterCommitCallback = std::function<void(xh256_t const &)>;

    // Commit iterates over all the children of a particular node, writes them out
    // to disk, forcefully tearing down all references in both directions. As a side
    // effect, all pre-images accumulated up to this point are also written.
    //
    // Note, this method is a non-synchronized mutator. It is unsafe to call this
    // concurrently with other mutators.
    void Commit(xh256_t const & hash, AfterCommitCallback cb, std::error_code & ec);

    void prune(xh256_t const & hash, std::error_code & ec);
    void commit_pruned(std::error_code & ec);
    void commit_pruned(std::unordered_set<xh256_t> const & pruned_hashes, std::error_code & ec);

    void prune(xh256_t const & root_key, std::vector<xh256_t> to_be_pruned_keys, std::error_code & ec);
    void commit_pruned(std::vector<xh256_t> pruned_root_hashes, std::error_code & ec);
    void clear_pruned(xh256_t const & root_hash, std::error_code & ec);
    void clear_pruned(std::error_code & ec);

    size_t pending_pruned_size(xh256_t const & root_hash) const noexcept;

private:
    // commit is the private locked version of Commit.
    void commit(xh256_t const & hash, std::map<xh256_t, xbytes_t> & data, AfterCommitCallback cb, std::error_code & ec);

    xbytes_t preimage_key(xh256_t const & hash_key) const;

    void cleans_put_lock_hold_outside(xh256_t const & hash, xbytes_t const & data);

    void cleans_erase_lock_hold_outside(xh256_t const & hash);
};
using xtrie_db_t = xtcash_trie_db;
using xtrie_db_ptr_t = std::shared_ptr<xtrie_db_t>;

class xtcash_trie_cache_node {
private:
    friend class xtcash_trie_db;

    xtrie_node_face_ptr_t node_;  // Cached collapsed trie node, or raw rlp data
    uint16_t size_;               // Byte size of the useful cached data

    uint32_t parents_;                         // Number of live nodes referencing this one
    std::map<xh256_t, uint16_t> children_;  // External children referenced by this node

    // xh256_t flush_prev_;  // Previous node in the flush-list
    // xh256_t flush_next_;  // Next node in the flush-list

private:
    xtcash_trie_cache_node(xtrie_node_face_ptr_t node, uint16_t _size/*, xh256_t _flushPrev*/) : node_{std::move(node)}, size_{_size}/*, flush_prev_{_flushPrev}*/ {
    }

private:
    // rlp returns the raw rlp encoded blob of the cached trie node, either directly
    // from the cache, or by regenerating it from the collapsed node.
    xbytes_t rlp();

    // obj returns the decoded and expanded trie node, either directly from the cache,
    // or by regenerating it from the rlp encoded blob.
    xtrie_node_face_ptr_t obj(xh256_t const & hash);

    using onChildFunc = std::function<void(xh256_t const &)>;

    // forChilds invokes the callback for all the tracked children of this node,
    // both the implicit ones from inside the node as well as the explicit ones
    // from outside the node.
    void forChilds(onChildFunc const & f);

    // forGatherChildren traverses the node hierarchy of a collapsed storage node and
    // invokes the callback for all the hashnode children.
    void forGatherChildren(xtrie_node_face_ptr_t n, onChildFunc f);
};
using xtrie_cache_node_t = xtcash_trie_cache_node;

// simplifyNode traverses the hierarchy of an expanded memory node and discards
// all the internal caches, returning a node that only contains the raw data.
xtrie_node_face_ptr_t simplify_node(xtrie_node_face_ptr_t const & n);

// expandNode traverses the node hierarchy of a collapsed storage node and converts
// all fields and keys into expanded memory form.
xtrie_node_face_ptr_t expandNode(std::shared_ptr<xtrie_hash_node_t> const & hash, xtrie_node_face_ptr_t n);

NS_END3
