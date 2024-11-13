// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "xbase/xns_macro.h"

#include <memory>

NS_BEG3(tcash, evm_common, trie)

// fwd:
class xtcash_trie_node_face;
using xtrie_node_face_t = xtcash_trie_node_face;
using xtrie_node_face_ptr_t = std::shared_ptr<xtrie_node_face_t>;

class xtcash_trie_hash_node;
using xtrie_hash_node_t = xtcash_trie_hash_node;
using xtrie_hash_node_ptr_t = std::shared_ptr<xtrie_hash_node_t>;

class xtcash_trie_value_node;
using xtrie_value_node_t = xtcash_trie_value_node;
using xtrie_value_node_ptr_t = std::shared_ptr<xtrie_value_node_t>;

class xtcash_trie_short_node;
using xtrie_short_node_t = xtcash_trie_short_node;
using xtrie_short_node_ptr_t = std::shared_ptr<xtrie_short_node_t>;

class xtcash_trie_full_node;
using xtrie_full_node_t = xtcash_trie_full_node;
using xtrie_full_node_ptr_t = std::shared_ptr<xtrie_full_node_t>;

class xtcash_trie_raw_node;
using xtrie_raw_node_t = xtcash_trie_raw_node;
using xtrie_raw_node_ptr_t = std::shared_ptr<xtrie_raw_node_t>;

class xtcash_trie_raw_full_node;
using xtrie_raw_full_node_t = xtcash_trie_raw_full_node;
using xtrie_raw_full_node_ptr_t = std::shared_ptr<xtrie_raw_full_node_t>;

class xtcash_trie_raw_short_node;
using xtrie_raw_short_node_t = xtcash_trie_raw_short_node;
using xtrie_raw_short_node_ptr_t = std::shared_ptr<xtrie_raw_short_node_t>;

NS_END3