// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xevm_common/trie/xtrie_db.h"

#include "xbasic/xhex.h"
#include "xevm_common/trie/xtrie_encoding.h"
#include "xevm_common/trie/xtrie_node.h"
#include "xevm_common/trie/xtrie_node_coding.h"
#include "xevm_common/xerror/xerror.h"
#include "xmetrics/xmetrics.h"

#include <cassert>

NS_BEG3(tcash, evm_common, trie)

constexpr uint32_t IdealBatchSize = 1024;

constexpr auto PreimagePrefix = ConstBytes<11>("secure-key-");

xtcash_trie_db::xtcash_trie_db(xkv_db_face_ptr_t diskdb, size_t const cache_size) : diskdb_{std::move(diskdb)}, cleans_{cache_size} {
}

std::shared_ptr<xtcash_trie_db> xtcash_trie_db::NewDatabase(xkv_db_face_ptr_t diskdb, size_t cache_size) {
    return std::make_shared<xtcash_trie_db>(std::move(diskdb), cache_size);
}

xkv_db_face_ptr_t const & xtcash_trie_db::DiskDB() const noexcept {
    return diskdb_;
}

void xtcash_trie_db::insert(xh256_t const & hash, int32_t const size, xtrie_node_face_ptr_t const & node) {
    std::lock_guard<std::mutex> lck(mutex_);
    // If the node's already cached, skip
    if (dirties_.find(hash) != dirties_.end()) {
        return;
    }

    // todo mark size.

    auto entry = xtrie_cache_node_t{simplify_node(node), static_cast<uint16_t>(size)/*, newest_*/};
    entry.forChilds([&](xh256_t const & child) {
        if (this->dirties_.find(child) != this->dirties_.end()) {
            this->dirties_.at(child).parents_++;
        }
    });
    xdbg("xtcash_trie_db::insert %s size:%d", hash.hex().c_str(), size);
    dirties_.insert({hash, entry});

    //if (oldest_.empty()) {
    //    oldest_ = hash;
    //} else {
    //    dirties_.at(newest_).flush_next_ = hash;
    //}
    //newest_ = hash;

    // todo dirties size;
}

void xtcash_trie_db::insertPreimage(xh256_t const & hash, xbytes_t const & preimage) {
    std::lock_guard<std::mutex> lck(mutex_);
    preimages_.insert({hash, preimage});
    // todo cal preimage size metrics.
}

xtrie_node_face_ptr_t xtcash_trie_db::node(xh256_t const & hash) {
    std::lock_guard<std::mutex> lck(mutex_);
    XMETRICS_GAUGE(metrics::mpt_trie_cache_visit, 1);
    // todo:
    xbytes_t value;
    if (cleans_.get(hash, value)) {
        // todo clean mark hit
        return xtrie_node_rlp::must_decode_node(hash, value);
    }
    if (dirties_.find(hash) != dirties_.end()) {
        // todo dirty mark hit
        return dirties_.at(hash).obj(hash);
    }
    // todo mark miss hit
    XMETRICS_GAUGE(metrics::mpt_trie_cache_miss, 1);
    // retrieve from disk db
    auto const & enc = ReadTrieNode(diskdb_, hash);
    if (enc.empty()) {
        return nullptr;
    }
    // put into clean cache
    cleans_put_lock_hold_outside(hash, enc);
    return xtrie_node_rlp::must_decode_node(hash, enc);
}

xbytes_t xtcash_trie_db::Node(xh256_t const & hash, std::error_code & ec) {
    std::lock_guard<std::mutex> lck(mutex_);
    // It doesn't make sense to retrieve the metaroot
    if (hash.empty()) {
        ec = error::xerrc_t::trie_db_not_found;
        return xbytes_t{};
    }

    // Retrieve the node from the clean cache if available
    xbytes_t value;
    if (cleans_.get(hash, value)) {
        return value;
    }

    // Retrieve the node from the dirty cache if available
    if (dirties_.find(hash) != dirties_.end()) {
        return dirties_.at(hash).rlp();
    }

    // Content unavailable in memory, attempt to retrieve from disk
    auto enc = ReadTrieNode(diskdb_, hash);
    if (enc.empty()) {
        ec = error::xerrc_t::trie_db_not_found;
        return xbytes_t{};
    }
    // put into clean cache
    cleans_put_lock_hold_outside(hash, enc);
    return enc;
}

xbytes_t xtcash_trie_db::preimage(xh256_t const & hash) const {
    std::lock_guard<std::mutex> lck(mutex_);
    if (preimages_.find(hash) != preimages_.end()) {
        return preimages_.at(hash);
    }
    // could put this diskdb->Get into trie_kv_db_face :: ReadPreimage
    std::error_code ec;
    auto result = diskdb_->get(preimage_key(hash), ec);
    if (ec) {
        xwarn("xtrie_db Get preimage %s failed: %s", hash.hex().c_str(), ec.message().c_str());
    }
    return result;
}

void xtcash_trie_db::Commit(xh256_t const & hash, AfterCommitCallback cb, std::error_code & ec) {
    assert(!ec);

    std::lock_guard<std::mutex> lck(mutex_);

    // what we can optimize here:
    // 1. use batch writer to sum all <k,v> into db with once writeDB operation
    // 2. db.preimages for secure trie
    // 3. use async cleaner to clean dirties value after commit.

    // 0. first, Move all of the accumulated preimages in.
    for (auto const & preimage : preimages_) {
        diskdb_->Put(preimage_key(preimage.first), preimage.second, ec);
        if (ec) {
            xwarn("xtrie_db Commit diskdb error at preimage %s, err: %s", preimage.first.hex().c_str(), ec.message().c_str());
            // return;
        }
        // todo could add some metrics here.
    }

    std::map<xh256_t, xbytes_t> batch;
    commit(hash, batch, cb, ec);
    if (ec) {
        xerror("xtcash_trie_db::Commit error: %s, %s", ec.category().name(), ec.message().c_str());
        return;
    }
    WriteTrieNodeBatch(diskdb_, batch);
    batch.clear();

    // todos: noted to remove size of dirty cache.
    // todos: change link-list of flush-list Next/Prev Node ptr
    // for now : we can simplily erase it:
    //newest_ = {};
    //oldest_ = {};

    // clean preimages:
    preimages_.clear();
}

void xtcash_trie_db::commit(xh256_t const & hash, std::map<xh256_t, xbytes_t> & data, AfterCommitCallback cb, std::error_code & ec) {
    // If the node does not exist, it's a previously committed node
    if (dirties_.find(hash) == dirties_.end()) {
        return;
    }

    auto node = dirties_.at(hash);
    node.forChilds([&](xh256_t const & child) {
        if (!ec) {
            this->commit(child, data, cb, ec);
        }
    });
    if (ec) {
        return;
    }

    // put it into batch
    auto enc = node.rlp();
    // auto const hash_bytes = to_bytes(hash);
    xdbg("xtcash_trie_db::Commit write node %s, size %zu", tcash::to_hex(hash).c_str(), enc.size());
    data.emplace(hash, enc);

    if (cb) {
        cb(hash);
    }
    if (data.size() >= IdealBatchSize) {
        WriteTrieNodeBatch(diskdb_, data);
        data.clear();
    }

    // clean dirties:
    dirties_.erase(hash);

    // and move it to cleans:
    cleans_put_lock_hold_outside(hash, enc);
    // todo mark size everywhere with cleans/dirties' insert/erase/...
}

void xtcash_trie_db::prune(xh256_t const & hash, std::error_code & ec) {
    std::lock_guard<std::mutex> lck(mutex_);
    {
        if (pruned_hashes_.find(hash) != std::end(pruned_hashes_)) {
            return;
        }

        cleans_erase_lock_hold_outside(hash);

        assert(dirties_.find(hash) == dirties_.end());

        pruned_hashes_.insert(hash);
    }
}

void xtcash_trie_db::commit_pruned(std::error_code & ec) {
    assert(!ec);
    std::vector<xspan_t<xbyte_t const>> pruned_keys;

    std::lock_guard<std::mutex> lck(mutex_);

    pruned_keys.reserve(pruned_hashes_.size());
    for (auto const & pruned_hash : pruned_hashes_) {
        pruned_keys.emplace_back(pruned_hash);
    }
    diskdb_->DeleteBatch(pruned_keys, ec);
    if (ec) {
        xwarn("pruning MPT nodes failed. %s", ec.message().c_str());
        return;
    }

    xkinfo("%zu keys pruned from disk", pruned_hashes_.size());

    pruned_hashes_.clear();
}

void xtcash_trie_db::commit_pruned(std::unordered_set<xh256_t> const & pruned_hashes, std::error_code & ec) {
    assert(!ec);
    std::vector<xspan_t<xbyte_t const>> pruned_keys;
    pruned_keys.reserve(pruned_hashes.size());

    std::lock_guard<std::mutex> lck(mutex_);
    for (auto const & hash : pruned_hashes) {
        cleans_erase_lock_hold_outside(hash);
        assert(dirties_.find(hash) == dirties_.end());
        pruned_keys.emplace_back(hash);
    }

    diskdb_->DeleteBatch(pruned_keys, ec);
    if (ec) {
        xwarn("pruning MPT nodes failed. %s", ec.message().c_str());
        return;
    }

    xkinfo("%zu keys pruned from disk", pruned_hashes_.size());

    pruned_hashes_.clear();
}

void xtcash_trie_db::prune(xh256_t const & root_key, std::vector<xh256_t> to_be_pruned_keys, std::error_code & ec) {
    assert(!ec);
    std::lock_guard<std::mutex> lck(mutex_);

    auto const it = pruned_hashes2_.find(root_key);
    if (it == std::end(pruned_hashes2_)) {
// #if defined(ENABLE_METRICS)
//         std::string const metrics_key = "triedb_prune_count_" + tcash::to_string(diskdb_->table_address());
//         XMETRICS_COUNTER_SET(metrics_key, to_be_pruned_keys.size());
// #endif
        pruned_hashes2_.emplace(root_key, std::move(to_be_pruned_keys));
        return;
    }

    ec = error::xerrc_t::trie_prune_data_duplicated;
}

void xtcash_trie_db::commit_pruned(std::vector<xh256_t> pruned_root_hashes, std::error_code & ec) {
    assert(!ec);

    std::lock_guard<std::mutex> lck(mutex_);

    pruned_root_hashes.erase(std::remove_if(std::begin(pruned_root_hashes),
                                            std::end(pruned_root_hashes),
                                            [this](xh256_t const & root_hash) { return pruned_hashes2_.find(root_hash) == std::end(pruned_hashes2_); }),
                             std::end(pruned_root_hashes));

    std::vector<xspan_t<xbyte_t const>> pruned_keys;
    size_t const pruned_keys_count = std::accumulate(
        std::begin(pruned_root_hashes), std::end(pruned_root_hashes), 0u, [this](size_t const cnt, xh256_t const & root_hash) { return cnt + pruned_hashes2_[root_hash].size(); });
    pruned_keys.reserve(pruned_keys_count);

    for (auto const & root_hash : pruned_root_hashes) {
        for (auto const & hash : pruned_hashes2_[root_hash]) {
            cleans_erase_lock_hold_outside(hash);
            assert(dirties_.find(hash) == dirties_.end());
            pruned_keys.emplace_back(hash);
        }
    }

    if (!pruned_keys.empty()) {
// #if defined(ENABLE_METRICS)
//         std::string const metrics_key = "triedb_commit_prune_count_" + tcash::to_string(diskdb_->table_address());
//         XMETRICS_COUNTER_SET(metrics_key, pruned_keys.size());
// #endif

        diskdb_->DeleteBatch(pruned_keys, ec);
        if (ec) {
            xwarn("pruning MPT nodes failed. errc %d msg %s", ec.value(), ec.message().c_str());
        }
    }

    for (auto const & root_hash : pruned_root_hashes) {
        pruned_hashes2_.erase(root_hash);
    }

// #if defined(ENABLE_METRICS)
//     size_t total_remaining_count{0};
//     for (auto const & pruned_hash_info : pruned_hashes2_) {
//         auto const & prune_ready = tcash::get<std::vector<xh256_t>>(pruned_hash_info);
//         total_remaining_count += prune_ready.size();
//     }

//     std::string const metrics_key = "triedb_pending_prune_count_" + tcash::to_string(diskdb_->table_address());
//     XMETRICS_COUNTER_SET(metrics_key, total_remaining_count);
// #endif
}

void xtcash_trie_db::clear_pruned(xh256_t const & root_hash, std::error_code & ec) {
    assert(!ec);

    std::lock_guard<std::mutex> lck(mutex_);

    auto const it = pruned_hashes2_.find(root_hash);
    if (it == std::end(pruned_hashes2_)) {
        ec = error::xerrc_t::trie_prune_data_not_found;
        xwarn("xtrie_db_t::clear_pruned fail to find pending root hash %s to be reverted", root_hash.hex().c_str());
        return;
    }

    pruned_hashes2_.erase(it);
}

void xtcash_trie_db::clear_pruned(XMAYBE_UNUSED std::error_code & ec) {
    assert(!ec);
    std::lock_guard<std::mutex> lck(mutex_);
    pruned_hashes2_.clear();
}

size_t xtcash_trie_db::pending_pruned_size(xh256_t const & root_hash) const noexcept {
    std::lock_guard<std::mutex> lck(mutex_);

    auto const it = pruned_hashes2_.find(root_hash);
    if (it == std::end(pruned_hashes2_)) {
        xwarn("xtrie_db_t::pruned_size returns 0 due to root hash %s not found", root_hash.hex().c_str());
        return 0 ;
    }

    return tcash::get<std::vector<xh256_t>>(*it).size();
}


xbytes_t xtcash_trie_db::preimage_key(xh256_t const & hash_key) const {
    xbytes_t res;
    res.insert(res.begin(), PreimagePrefix.begin(), PreimagePrefix.end());
    res.insert(res.end(), hash_key.begin(), hash_key.end());
    return res;
}

void xtcash_trie_db::cleans_put_lock_hold_outside(xh256_t const & hash, xbytes_t const & data) {
#if defined(CACHE_SIZE_STATISTIC) || defined(CACHE_SIZE_STATISTIC_MORE_DETAIL)
    auto erased_vec = cleans_.put(hash, data);
    int32_t inc_num = 1 - (int32_t)erased_vec.size();
    // list_node_size: sizeof(xh256_t) + sizeof(xbytes_t) + 16(pre and next pointer) = 32 + 24 + 16 = 72
    // unorderd_map_node_size : 24
    // here use data.size() because lru put(const KeyT & key, const ValueT & value) will copy data to value in cache.
    // is use std::move(data), here should use capacity.
    int32_t inc_size = 96*inc_num + data.size()*sizeof(xbyte_t);

    for (auto & erased : erased_vec) {
        inc_size -= (int32_t)(erased.second.capacity()*sizeof(xbyte_t));
        // xdbg("-----cache size lru---- xtcash_trie_db erase hash:%s,data capacity:%d size:%d", tcash::to_hex(erased.first).c_str(), erased.second.capacity(), erased.second.size());
    }
    // xdbg("-----cache size lru---- xtcash_trie_db add hash:%s,data capacity:%d size:%d", tcash::to_hex(hash).c_str(), data.capacity(), data.size());
    // xdbg("-----cache size lru----- xtcash_trie_db num:%d size:%d", inc_num, inc_size);
    XMETRICS_GAUGE(metrics::statistic_mpt_node_cache_num, inc_num);
    XMETRICS_GAUGE(metrics::statistic_mpt_node_cache_size, inc_size);
    XMETRICS_GAUGE(metrics::statistic_total_size, inc_size);
#else
    cleans_.put(hash, data);
#endif
}

void xtcash_trie_db::cleans_erase_lock_hold_outside(xh256_t const & hash) {
#if defined(CACHE_SIZE_STATISTIC) || defined(CACHE_SIZE_STATISTIC_MORE_DETAIL)
    auto erased_vec = cleans_.erase(hash);
    if (erased_vec.empty()) {
        return;
    }
    XMETRICS_GAUGE(metrics::statistic_mpt_node_cache_num, -1);
    // list_node_size: sizeof(xh256_t) + sizeof(xbytes_t) + 16(pre and next pointer) = 32 + 24 + 16 = 72
    // unorderd_map_node_size : 24
    int32_t dec_size = (int32_t)(96 + erased_vec[0].second.capacity()*sizeof(xbyte_t));
    // xdbg("-----cache size lru----- xtcash_trie_db erase hash:%s,data capacity:%d size:%d", tcash::to_hex(erased_vec[0].first).c_str(), erased_vec[0].second.capacity(), erased_vec[0].second.size());
    XMETRICS_GAUGE(metrics::statistic_mpt_node_cache_size, -dec_size);
    XMETRICS_GAUGE(metrics::statistic_total_size, -dec_size);
#else
    cleans_.erase(hash);
    xdbg("erased hash %s from clean", hash.hex().c_str());
#endif
}

// ============
// trie cache
// ============
xbytes_t xtrie_cache_node_t::rlp() {
    if (node_ == nullptr) {
        return xbytes_t{};
    }
    // if (node->type() == xtrie_node_type_t::rawnode) {
    //     auto n = std::dynamic_pointer_cast<xtrie_raw_node_t>(node);
    //     return n->data();
    // }
    return xtrie_node_rlp::EncodeToBytes(node_);
}

xtrie_node_face_ptr_t xtrie_cache_node_t::obj(xh256_t const & hash) {
    if (node_ == nullptr) {
        return node_;
    }
    if (node_->type() == xtrie_node_type_t::rawnode) {
        auto n = std::dynamic_pointer_cast<xtrie_raw_node_t>(node_);
        assert(n != nullptr);

        return xtrie_node_rlp::must_decode_node(hash, n->data());
    }
    return expandNode(std::make_shared<xtrie_hash_node_t>(hash), node_);
}

void xtrie_cache_node_t::forChilds(onChildFunc const & f) {
    for (auto const & c : children_) {
        f(c.first);
    }
    if (node_ && node_->type() != xtrie_node_type_t::rawnode) {
        forGatherChildren(node_, f);
    }
}

void xtrie_cache_node_t::forGatherChildren(xtrie_node_face_ptr_t n, onChildFunc f) {
    if (n == nullptr) {
        return;
    }
    switch (n->type()) {
    case xtrie_node_type_t::rawshortnode: {
        auto nn = std::dynamic_pointer_cast<xtrie_raw_short_node_t>(n);
        assert(nn != nullptr);

        forGatherChildren(nn->Val, f);
        return;
    }
    case xtrie_node_type_t::rawfullnode: {
        auto nn = std::dynamic_pointer_cast<xtrie_raw_full_node_t>(n);
        assert(nn != nullptr);

        for (std::size_t index = 0; index < 16; ++index) {
            forGatherChildren(nn->Children[index], f);
        }
        return;
    }
    case xtrie_node_type_t::hashnode: {
        auto const nn = std::dynamic_pointer_cast<xtrie_hash_node_t>(n);
        assert(nn != nullptr);

        f(nn->data());
        return;
    }
    case xtrie_node_type_t::valuenode:
        XATTRIBUTE_FALLTHROUGH;
    case xtrie_node_type_t::rawnode:
        // pass;
        return;
    default: {
        xerror("unknown node type: %d", n->type());
        xassert(false);
    }
    }
    __builtin_unreachable();
}

// static methods:
xtrie_node_face_ptr_t simplify_node(xtrie_node_face_ptr_t const & n) {
    switch (n->type()) {  // NOLINT(clang-diagnostic-switch-enum)
    case xtrie_node_type_t::shortnode: {
        auto const node = std::dynamic_pointer_cast<xtrie_short_node_t>(n);
        assert(node != nullptr);

        return std::make_shared<xtrie_raw_short_node_t>(node->key, simplify_node(node->val));
    }
    case xtrie_node_type_t::fullnode: {
        auto const node = std::dynamic_pointer_cast<xtrie_full_node_t>(n);
        assert(node != nullptr);

        auto raw_fullnode_ptr = std::make_shared<xtrie_raw_full_node_t>(node->children);
        for (std::size_t i = 0; i < raw_fullnode_ptr->Children.size(); ++i) {
            if (raw_fullnode_ptr->Children[i] != nullptr) {
                raw_fullnode_ptr->Children[i] = simplify_node(raw_fullnode_ptr->Children[i]);
            }
        }
        return raw_fullnode_ptr;
    }

    case xtrie_node_type_t::valuenode:  // NOLINT(bugprone-branch-clone)
        XATTRIBUTE_FALLTHROUGH;
    case xtrie_node_type_t::hashnode:
        XATTRIBUTE_FALLTHROUGH;
    case xtrie_node_type_t::rawnode:
        return n;

    default: {
        xerror("unknown node type: %d", n->type());
        assert(false);  // NOLINT(clang-diagnostic-disabled-macro-expansion)
        return nullptr;
    }
    }
}

xtrie_node_face_ptr_t expandNode(std::shared_ptr<xtrie_hash_node_t> const & hash, xtrie_node_face_ptr_t n) {
    switch (n->type()) {
    case xtrie_node_type_t::rawshortnode: {
        auto node = std::dynamic_pointer_cast<xtrie_raw_short_node_t>(n);
        assert(node != nullptr);

        return std::make_shared<xtrie_short_node_t>(compact_to_hex(node->Key), expandNode(nullptr, node->Val), xnode_flag_t{hash});
    }
    case xtrie_node_type_t::rawfullnode: {
        auto node = std::dynamic_pointer_cast<xtrie_raw_full_node_t>(n);
        assert(node != nullptr);

        auto fullnode_ptr = std::make_shared<xtrie_full_node_t>(xnode_flag_t{hash});
        for (std::size_t i = 0; i < fullnode_ptr->children.size(); ++i) {
            if (node->Children[i] != nullptr) {
                fullnode_ptr->children[i] = expandNode(nullptr, node->Children[i]);
            }
        }
        return fullnode_ptr;
    }
    case xtrie_node_type_t::valuenode:
        XATTRIBUTE_FALLTHROUGH;
    case xtrie_node_type_t::hashnode:
        return n;
    default: {
        xerror("unknown node type: %d", n->type());
        xassert(false);
    }
    }
    __builtin_unreachable();
}

NS_END3
