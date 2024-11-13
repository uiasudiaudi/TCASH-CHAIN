// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"
#include "xbasic/xfixed_hash.h"
#include "xbasic/xspan.h"
#include "xevm_common/rlp/xrlp_encodable.h"
#include "xevm_common/trie/xtrie_node_fwd.h"

#include <array>
#include <memory>
#include <string>
#include <utility>

NS_BEG3(tcash, evm_common, trie)

enum class xtcash_trie_node_type : uint8_t {
    invalid = 0,
    fullnode = 1,
    shortnode = 2,
    hashnode = 3,
    valuenode = 4,

    // rawNode is a simple binary blob used to differentiate between collapsed trie
    // nodes and already encoded RLP binary blobs (while at the same time store them
    // in the same cache fields).
    rawnode = 5,

    // rawFullNode represents only the useful data content of a full node, with the
    // caches and flags stripped out to minimize its data storage. This type honors
    // the same RLP encoding as the original parent.
    rawfullnode = 6,

    // rawShortNode represents only the useful data content of a short node, with the
    // caches and flags stripped out to minimize its data storage. This type honors
    // the same RLP encoding as the original parent.
    rawshortnode = 7,
};
using xtrie_node_type_t = xtcash_trie_node_type;

struct xtcash_node_flag {  // NOLINT(clang-diagnostic-padded)
private:
    std::shared_ptr<xtrie_hash_node_t> hash_node_{nullptr};
    bool dirty_{false};

public:
    xtcash_node_flag() = default;
    xtcash_node_flag(xtcash_node_flag const &) = default;
    xtcash_node_flag & operator=(xtcash_node_flag const &) = default;
    xtcash_node_flag(xtcash_node_flag &&) = default;
    xtcash_node_flag & operator=(xtcash_node_flag &&) = default;
    ~xtcash_node_flag() = default;

    xtcash_node_flag(std::shared_ptr<xtrie_hash_node_t> hash, bool dirty);
    explicit xtcash_node_flag(std::shared_ptr<xtrie_hash_node_t> hash);
    explicit xtcash_node_flag(bool dirty);

    std::shared_ptr<xtrie_hash_node_t> const & hash_node() const noexcept;
    bool dirty() const noexcept;

    void hash_node(std::shared_ptr<xtrie_hash_node_t> node) noexcept;
    void dirty(bool dirty) noexcept;
};
using xnode_flag_t = xtcash_node_flag;
using xtrie_node_cached_data_t = xtcash_node_flag;

class xtcash_trie_node_face {
public:
#if defined(ENABLE_METRICS)
    xtcash_trie_node_face() noexcept;
    xtcash_trie_node_face(xtcash_trie_node_face const &);
    xtcash_trie_node_face & operator=(xtcash_trie_node_face const &) = default;
    xtcash_trie_node_face(xtcash_trie_node_face &&) noexcept;
    xtcash_trie_node_face & operator=(xtcash_trie_node_face &&) = default;
    virtual ~xtcash_trie_node_face() noexcept;
#else
    xtcash_trie_node_face() = default;
    xtcash_trie_node_face(xtcash_trie_node_face const &) = default;
    xtcash_trie_node_face & operator=(xtcash_trie_node_face const &) = default;
    xtcash_trie_node_face(xtcash_trie_node_face &&) = default;
    xtcash_trie_node_face & operator=(xtcash_trie_node_face &&) = default;
    virtual ~xtcash_trie_node_face() = default;
#endif

    virtual std::string fstring(std::string const & ind) const = 0;
    virtual xtrie_node_cached_data_t cache() const = 0;
    virtual xtrie_node_type_t type() const noexcept = 0;
};
using xtrie_node_face_t = xtcash_trie_node_face;
using xtrie_node_face_ptr_t = std::shared_ptr<xtrie_node_face_t>;

// value&&hash node
class xtcash_trie_hash_node : public xtrie_node_face_t {
private:
    xh256_t hash_{};

public:
    xtcash_trie_hash_node() = default;
    xtcash_trie_hash_node(xtcash_trie_hash_node const &) = default;
    xtcash_trie_hash_node & operator=(xtcash_trie_hash_node const &) = default;
    xtcash_trie_hash_node(xtcash_trie_hash_node &&) = default;
    xtcash_trie_hash_node & operator=(xtcash_trie_hash_node &&) = default;
    ~xtcash_trie_hash_node() override = default;

    explicit xtcash_trie_hash_node(xbytes_t const & hash_data);
    explicit xtcash_trie_hash_node(xspan_t<xbyte_t const> hash_data);

    xh256_t const & data() const noexcept;

    std::string fstring(std::string const & ind) const override;
    xtrie_node_cached_data_t cache() const override;
    xtrie_node_type_t type() const noexcept override;
};
using xtrie_hash_node_t = xtcash_trie_hash_node;
using xtrie_hash_node_ptr_t = std::shared_ptr<xtrie_hash_node_t>;

class xtcash_trie_value_node : public xtrie_node_face_t {
private:
    xbytes_t m_data;

public:
    xtcash_trie_value_node() = default;
    xtcash_trie_value_node(xtcash_trie_value_node const &) = default;
    xtcash_trie_value_node & operator=(xtcash_trie_value_node const &) = default;
    xtcash_trie_value_node(xtcash_trie_value_node &&) = default;
    xtcash_trie_value_node & operator=(xtcash_trie_value_node &&) = default;
    ~xtcash_trie_value_node() override = default;

    explicit xtcash_trie_value_node(xbytes_t data);

    xbytes_t const & data() const noexcept;

    std::string fstring(std::string const & ind) const override;
    xtrie_node_cached_data_t cache() const override;
    xtrie_node_type_t type() const noexcept override;
};
using xtrie_value_node_t = xtcash_trie_value_node;
using xtrie_value_node_ptr_t = std::shared_ptr<xtrie_value_node_t>;

// nilValueNode is used when collapsing internal trie nodes for hashing, since
// unset children need to serialize correctly.
static const xtrie_value_node_t nilValueNode{};

// for leaf node && extension node
class xtcash_trie_short_node : public xtrie_node_face_t, public rlp::xrlp_encodable_t<xtcash_trie_short_node> {
public:
    xbytes_t key{};
    xtrie_node_face_ptr_t val{};
    xnode_flag_t flags{};

public:
    xtcash_trie_short_node() = default;
    xtcash_trie_short_node(xtcash_trie_short_node const &) = default;
    xtcash_trie_short_node & operator=(xtcash_trie_short_node const &) = default;
    xtcash_trie_short_node(xtcash_trie_short_node &&) = default;
    xtcash_trie_short_node & operator=(xtcash_trie_short_node &&) = default;
    ~xtcash_trie_short_node() override = default;

    xtcash_trie_short_node(xbytes_t key, xtrie_node_face_ptr_t val, xnode_flag_t flag);

    std::shared_ptr<xtcash_trie_short_node> clone() const;

    std::string fstring(std::string const & ind) const override;
    xtrie_node_cached_data_t cache() const override;
    xtrie_node_type_t type() const noexcept override;

    void EncodeRLP(xbytes_t & buf, std::error_code & ec) override;
};
using xtrie_short_node_t = xtcash_trie_short_node;
using xtrie_short_node_ptr_t = std::shared_ptr<xtrie_short_node_t>;

// for branch node
// actual trie node data to encode/decode, so it need to impl encodeELP
class xtcash_trie_full_node
  : public xtrie_node_face_t
  , public rlp::xrlp_encodable_t<xtcash_trie_full_node> {
public:
    std::array<xtrie_node_face_ptr_t, 17> children;
    xnode_flag_t flags;

public:
    xtcash_trie_full_node() = default;
    xtcash_trie_full_node(xtcash_trie_full_node const &) = default;
    xtcash_trie_full_node & operator=(xtcash_trie_full_node const &) = default;
    xtcash_trie_full_node(xtcash_trie_full_node &&) = default;
    xtcash_trie_full_node & operator=(xtcash_trie_full_node &&) = default;
    ~xtcash_trie_full_node() override = default;

    explicit xtcash_trie_full_node(xnode_flag_t f);

    std::shared_ptr<xtcash_trie_full_node> clone() const;

    std::string fstring(std::string const & ind) const override;
    xtrie_node_cached_data_t cache() const override;
    xtrie_node_type_t type() const noexcept override;

    void EncodeRLP(xbytes_t & buf, std::error_code & ec) override;
};
using xtrie_full_node_t = xtcash_trie_full_node;
using xtrie_full_node_ptr_t = std::shared_ptr<xtrie_full_node_t>;

/// ? only used by database? xtrie_db.cpp
class xtcash_trie_raw_node : public xtrie_node_face_t {
private:
    xbytes_t m_data;

public:
    xtcash_trie_raw_node() = default;
    xtcash_trie_raw_node(xtcash_trie_raw_node const &) = default;
    xtcash_trie_raw_node & operator=(xtcash_trie_raw_node const &) = default;
    xtcash_trie_raw_node(xtcash_trie_raw_node &&) = default;
    xtcash_trie_raw_node & operator=(xtcash_trie_raw_node &&) = default;
    ~xtcash_trie_raw_node() override = default;

    explicit xtcash_trie_raw_node(xbytes_t const & data) : m_data{data} {
    }

public:
    xbytes_t const & data() const noexcept {
        return m_data;
    }

public:
    std::string fstring(std::string const & ind) const override {
        xassert(false);
        // should not used
        return "";
    }
    xtrie_node_cached_data_t cache() const override {
        xassert(false);
        // should not used
        return {{}, true};
    }
    xtrie_node_type_t type() const noexcept override {
        return xtrie_node_type_t::rawnode;
    }
};
using xtrie_raw_node_t = xtcash_trie_raw_node;
using xtrie_raw_node_ptr_t = std::shared_ptr<xtrie_raw_node_t>;

class xtcash_trie_raw_full_node
  : public xtrie_node_face_t
  , public rlp::xrlp_encodable_t<xtcash_trie_raw_full_node> {
public:
    std::array<xtrie_node_face_ptr_t, 17> Children;

public:
    xtcash_trie_raw_full_node(std::array<xtrie_node_face_ptr_t, 17> const & child) {
        Children = child;
    }

public:
    std::string fstring(std::string const & ind) const override {
        xassert(false);
        // should not used
        return "";
    }
    xtrie_node_cached_data_t cache() const override {
        xassert(false);
        // should not used
        return {{}, true};
    }
    xtrie_node_type_t type() const noexcept override {
        return xtrie_node_type_t::rawfullnode;
    }

public:
    void EncodeRLP(xbytes_t & buf, std::error_code & ec);
};
using xtrie_raw_full_node_t = xtcash_trie_raw_full_node;
using xtrie_raw_full_node_ptr_t = std::shared_ptr<xtrie_raw_full_node_t>;

class xtcash_trie_raw_short_node
  : public xtrie_node_face_t
  , public rlp::xrlp_encodable_t<xtcash_trie_raw_full_node> {
public:
    xbytes_t Key;
    xtrie_node_face_ptr_t Val;

public:
    xtcash_trie_raw_short_node(xbytes_t key, xtrie_node_face_ptr_t val) : Key{std::move(key)}, Val{std::move(val)} {
    }

public:
    std::string fstring(std::string const & ind) const override {
        xassert(false);
        // should not used
        return "";
    }
    xtrie_node_cached_data_t cache() const override {
        xassert(false);
        // should not used
        return {{}, true};
    }
    xtrie_node_type_t type() const noexcept override {
        return xtrie_node_type_t::rawshortnode;
    }

public:
    void EncodeRLP(xbytes_t & buf, std::error_code & ec);
};
using xtrie_raw_short_node_t = xtcash_trie_raw_short_node;
using xtrie_raw_short_node_ptr_t = std::shared_ptr<xtrie_raw_short_node_t>;

NS_END3
