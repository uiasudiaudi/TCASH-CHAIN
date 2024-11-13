// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xevm_common/trie/xsecure_trie.h"

#include "xbasic/xstring.h"
#include "xutility/xhash.h"

NS_BEG3(tcash, evm_common, trie)

xtcash_secure_trie::xtcash_secure_trie(std::unique_ptr<xtrie_t> trie) : trie_{std::move(trie)} {
}

std::unique_ptr<xtcash_secure_trie> xtcash_secure_trie::build_from(xh256_t const & root, observer_ptr<xtrie_db_t> const db, std::error_code & ec) {
    assert(!ec);

    if (db == nullptr) {
        xerror("build secure trie from null db");
    }
    auto trie = xtrie_t::build_from(root, db, ec);
    if (ec) {
        xwarn("secure trie new failed: %s", ec.message().c_str());
        return nullptr;
    }
    return std::unique_ptr<xtcash_secure_trie>(new xtcash_secure_trie{std::move(trie)});
}

xbytes_t xtcash_secure_trie::get(xbytes_t const & key) const {
    std::error_code ec;
    auto result = try_get(key, ec);
    if (ec) {
        xerror("secure trie error: %s", ec.message().c_str());
    }
    return result;
}

xbytes_t xtcash_secure_trie::try_get(xbytes_t const & key, std::error_code & ec) const {
    assert(trie_ != nullptr);
    return trie_->try_get(hash_key(key), ec);
}


//std::pair<xbytes_t, std::size_t> xtcash_secure_trie::try_get_node(xbytes_t const & path, std::error_code & ec) const {
//    assert(m_trie != nullptr);
//    return m_trie->try_get_node(path, ec);
//}

void xtcash_secure_trie::update(xbytes_t const & key, xbytes_t const & value) {
    std::error_code ec;
    try_update(key, value, ec);
    if (ec) {
        xerror("secure trie error: %s", ec.message().c_str());
    }
    return;
}

void xtcash_secure_trie::try_update(xbytes_t const & key, xbytes_t const & value, std::error_code & ec) {
    auto const hk = hash_key(key);
    assert(trie_ != nullptr);

    trie_->try_update(hk, value, ec);
    //if (ec) {
    //    return;
    //}

    // (*get_sec_key_cache())[tcash::to_string(hk)] = key;
}

void xtcash_secure_trie::Delete(xbytes_t const & key) {
    std::error_code ec;
    try_delete(key, ec);
    if (ec) {
        xerror("secure trie error: %s", ec.message().c_str());
    }
    return;
}

void xtcash_secure_trie::try_delete(xbytes_t const & key, std::error_code & ec) {
    auto hk = hash_key(key);
    // get_sec_key_cache()->erase(tcash::to_string(hk));
    assert(trie_ != nullptr);
    trie_->try_delete(hk, ec);
}

//xbytes_t xtcash_secure_trie::get_key(xbytes_t const & shaKey) {
//    auto sc = get_sec_key_cache();
//    if (sc->find(tcash::to_string(shaKey)) != sc->end()) {
//        return sc->at(tcash::to_string(shaKey));
//    }
//    xdbg("xtcash_secure_trie::GetKey find key from trie_db preimage");
//    assert(trie_ != nullptr);
//    return trie_->trie_db()->preimage(xh256_t{shaKey});
//}

std::pair<xh256_t, int32_t> xtcash_secure_trie::commit(std::error_code & ec) {
    assert(trie_ != nullptr);
    // Write all the pre-images to the actual disk database
    //auto const sc = get_sec_key_cache();
    //if (!sc->empty()) {
    //     for (auto const & scp : *sc) {
    //         trie_->trie_db()->insertPreimage(xh256_t{tcash::to_bytes(tcash::get<std::string const>(scp))}, tcash::get<xbytes_t>(scp));
    //     }
    //    sc->clear();
    //}
    return trie_->commit(ec);
}

xh256_t xtcash_secure_trie::hash() {
    assert(trie_ != nullptr);
    return trie_->hash();
}

xbytes_t xtcash_secure_trie::hash_key(xbytes_t const & key) const {
    xdbg("xtcash_secure_trie::hashKey hashData:(%zu) %s ", key.size(), tcash::to_hex(key).c_str());
    xbytes_t hashbuf;
    utl::xkeccak256_t hasher;
    hasher.update(key.data(), key.size());
    hasher.get_hash(hashbuf);
    xdbg("xtcash_secure_trie::hashKey -> hashed data:(%zu) %s", hashbuf.size(), tcash::to_hex(hashbuf).c_str());
    return hashbuf;
}

void xtcash_secure_trie::prune(xh256_t const & old_trie_root_hash, std::unordered_set<xh256_t> & pruned_hashes, std::error_code & ec) {
    assert(!ec);
    assert(trie_ != nullptr);

    trie_->prune(old_trie_root_hash, pruned_hashes, ec);
}

void xtcash_secure_trie::commit_pruned(std::unordered_set<xh256_t> const & pruned_hashes, std::error_code & ec) {
    assert(!ec);
    assert(trie_ != nullptr);

    trie_->commit_pruned(pruned_hashes, ec);
}

void xtcash_secure_trie::prune(std::error_code & ec) {
    assert(!ec);
    assert(trie_ != nullptr);

    trie_->prune(ec);
}

void xtcash_secure_trie::commit_pruned(std::vector<xh256_t> pruned_root_hashes, std::error_code & ec) {
    assert(!ec);
    assert(trie_ != nullptr);
    trie_->commit_pruned(std::move(pruned_root_hashes), ec);
}

void xtcash_secure_trie::clear_pruned(xh256_t const & pending_pruned_trie_root_hash, std::error_code & ec) {
    assert(!ec);
    assert(trie_ != nullptr);
    trie_->clear_pruned(pending_pruned_trie_root_hash, ec);
}

void xtcash_secure_trie::clear_pruned(std::error_code & ec) {
    assert(!ec);
    assert(trie_ != nullptr);
    trie_->clear_pruned(ec);
}


xh256_t const & xtcash_secure_trie::original_root_hash() const noexcept {
    return trie_->original_root_hash();
}

NS_END3
