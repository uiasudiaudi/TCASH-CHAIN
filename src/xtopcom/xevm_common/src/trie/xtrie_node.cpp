// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xevm_common/trie/xtrie_node.h"

#include "xbasic/xhex.h"
#include "xcommon/rlp.h"
#if defined(ENABLE_METRICS)
#include "xmetrics/xmetrics.h"
#endif

#include <cassert>
#include <sstream>

NS_BEG3(tcash, evm_common, trie)

static char const * indices[]{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "[17]"};

xtcash_node_flag::xtcash_node_flag(std::shared_ptr<xtrie_hash_node_t> hash, bool const dirty) : hash_node_{std::move(hash)}, dirty_{dirty} {
}

xtcash_node_flag::xtcash_node_flag(std::shared_ptr<xtrie_hash_node_t> hash) : xtcash_node_flag{std::move(hash), false} {
}

xtcash_node_flag::xtcash_node_flag(bool const dirty) : xtcash_node_flag{nullptr, dirty} {
}

std::shared_ptr<xtrie_hash_node_t> const & xtcash_node_flag::hash_node() const noexcept {
    return hash_node_;
}

bool xtcash_node_flag::dirty() const noexcept {
    return dirty_;
}

void xtcash_node_flag::hash_node(std::shared_ptr<xtrie_hash_node_t> node) noexcept {
    hash_node_ = std::move(node);
}
void xtcash_node_flag::dirty(bool const dirty) noexcept {
    dirty_ = dirty;
}

#if defined(ENABLE_METRICS)

xtcash_trie_node_face::xtcash_trie_node_face() noexcept {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_mpt_trie_node_cnt, 1);
}

xtcash_trie_node_face::xtcash_trie_node_face(xtcash_trie_node_face const &) {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_mpt_trie_node_cnt, 1);
}

xtcash_trie_node_face::xtcash_trie_node_face(xtcash_trie_node_face &&) noexcept {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_mpt_trie_node_cnt, 1);
}

xtcash_trie_node_face::~xtcash_trie_node_face() noexcept {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_mpt_trie_node_cnt, -1);
}

#endif

xtcash_trie_hash_node::xtcash_trie_hash_node(xbytes_t const & hash_data) : hash_{hash_data} {
}

xtcash_trie_hash_node::xtcash_trie_hash_node(xspan_t<xbyte_t const> const hash_data) : hash_{hash_data} {
}

xh256_t const & xtcash_trie_hash_node::data() const noexcept {
    return hash_;
}

std::string xtcash_trie_hash_node::fstring(std::string const & ind) const {
    // return fmt.Sprintf("<%x> ", []byte(n))
    std::ostringstream resp;
    resp << "<" << to_hex(hash_) << "> ";
    return resp.str();
}

xtrie_node_cached_data_t xtcash_trie_hash_node::cache() const {
    return {nullptr, true};
}

xtrie_node_type_t xtcash_trie_hash_node::type() const noexcept {
    return xtrie_node_type_t::hashnode;
}

xtcash_trie_value_node::xtcash_trie_value_node(xbytes_t data)
    : m_data{std::move(data)} {
}

xbytes_t const & xtcash_trie_value_node::data() const noexcept {
    return m_data;
}

std::string xtcash_trie_value_node::fstring(std::string const & ind) const {
    // return fmt.Sprintf("%x ", []byte(n))
    std::ostringstream resp;
    resp << to_hex(m_data) << ' ';
    return resp.str();
}

xtrie_node_cached_data_t xtcash_trie_value_node::cache() const {
    return {nullptr, true};
}

xtrie_node_type_t xtcash_trie_value_node::type() const noexcept {
    return xtrie_node_type_t::valuenode;
}

xtcash_trie_short_node::xtcash_trie_short_node(xbytes_t _key, xtrie_node_face_ptr_t _val, xnode_flag_t flag)
    : key{std::move(_key)}, val{std::move(_val)}, flags{std::move(flag)} {
}

std::shared_ptr<xtcash_trie_short_node> xtcash_trie_short_node::clone() const {
    return std::make_shared<xtcash_trie_short_node>(*this);
}

std::string xtcash_trie_short_node::fstring(std::string const & ind) const {
    // return fmt.Sprintf("{%x: %v} ", n.Key, n.Val.fstring(ind+"  "))
    std::ostringstream resp;
    resp << '{' << to_hex(key) << ": " << val->fstring(ind + "  ") << "} ";
    return resp.str();
}

xtrie_node_cached_data_t xtcash_trie_short_node::cache() const {
    return flags;
}

xtrie_node_type_t xtcash_trie_short_node::type() const noexcept {
    return xtrie_node_type_t::shortnode;
}

void xtcash_trie_short_node::EncodeRLP(xbytes_t & buf, std::error_code & ec) {
    xbytes_t encoded;
    encoded.reserve(1024);

    append(encoded, RLP::encode(key));

    switch (val->type()) {  // NOLINT(clang-diagnostic-switch-enum)
    case xtrie_node_type_t::hashnode: {
        assert(dynamic_cast<xtrie_hash_node_t *>(val.get()) != nullptr);
        append(encoded, RLP::encode(xspan_t<xbyte_t const>{std::dynamic_pointer_cast<xtrie_hash_node_t>(val)->data()}));

        break;
    }

    case xtrie_node_type_t::valuenode: {
        assert(dynamic_cast<xtrie_value_node_t *>(val.get()) != nullptr);
        append(encoded, RLP::encode(std::dynamic_pointer_cast<xtrie_value_node_t>(val)->data()));

        break;
    }

    case xtrie_node_type_t::fullnode: {
        assert(dynamic_cast<xtrie_full_node_t *>(val.get()) != nullptr);
        std::dynamic_pointer_cast<xtrie_full_node_t>(val)->EncodeRLP(encoded, ec);

        break;
    }

    default:
        assert(false);  // NOLINT(clang-diagnostic-disabled-macro-expansion)
        xwarn("!!!! shortnode not encode type: %d", static_cast<uint8_t>(val->type()));

        break;
    }

    append(buf, RLP::encodeList(encoded));
}

xtcash_trie_full_node::xtcash_trie_full_node(xnode_flag_t f) : flags{std::move(f)} {
}

std::shared_ptr<xtcash_trie_full_node> xtcash_trie_full_node::clone() const {
    return std::make_shared<xtcash_trie_full_node>(*this);
}

std::string xtcash_trie_full_node::fstring(std::string const & ind) const {
    /*
     * resp := fmt.Sprintf("[\n%s  ", ind)
	for i, node := range &n.Children {
		if node == nil {
			resp += fmt.Sprintf("%s: <nil> ", indices[i])
		} else {
			resp += fmt.Sprintf("%s: %v", indices[i], node.fstring(ind+"  "))
		}
	}
	return resp + fmt.Sprintf("\n%s] ", ind)
     */
    std::ostringstream resp;
    resp << "[\n  " << ind;
    for (auto i = 0u; i < children.size(); ++i) {
        auto const & child = children[i];
        if (child == nullptr) {
            resp << indices[i] << ": <nil> ";
        } else {
            resp << indices[i] << ": " << child->fstring(ind + "  ");
        }
    }
    resp << "\n" << ind << "] ";
    return resp.str();
}

xtrie_node_cached_data_t xtcash_trie_full_node::cache() const {
    return flags;
}

xtrie_node_type_t xtcash_trie_full_node::type() const noexcept {
    return xtrie_node_type_t::fullnode;
}

void xtcash_trie_full_node::EncodeRLP(xbytes_t & buf, std::error_code & ec) {
    xbytes_t encoded;
    encoded.reserve(1024);

    for (auto const & child : children) {
        if (child == nullptr) {
            append(encoded, RLP::encode(nilValueNode.data()));  // 0x80 for empty bytes.
            continue;
        }

        switch (child->type()) {  // NOLINT(clang-diagnostic-switch-enum)
        case xtrie_node_type_t::hashnode: {
            assert(dynamic_cast<xtrie_hash_node_t *>(child.get()) != nullptr);
            append(encoded, RLP::encode(xspan_t<xbyte_t const>{std::dynamic_pointer_cast<xtrie_hash_node_t>(child)->data()}));

            break;
        }

        case xtrie_node_type_t::valuenode: {
            assert(dynamic_cast<xtrie_value_node_t *>(child.get()) != nullptr);
            append(encoded, RLP::encode(std::dynamic_pointer_cast<xtrie_value_node_t>(child)->data()));

            break;
        }

        case xtrie_node_type_t::fullnode: {
            assert(dynamic_cast<xtrie_full_node_t *>(child.get()) != nullptr);
            std::dynamic_pointer_cast<xtrie_full_node_t>(child)->EncodeRLP(encoded, ec);

            break;
        }

        case xtrie_node_type_t::shortnode: {
            assert(dynamic_cast<xtrie_short_node_t *>(child.get()) != nullptr);
            std::dynamic_pointer_cast<xtrie_short_node_t>(child)->EncodeRLP(encoded, ec);

            break;
        }

        default:
            assert(false);  // NOLINT(clang-diagnostic-disabled-macro-expansion)
            xwarn("!!! full node not encode child type: %d", static_cast<uint8_t>(child->type()));

            break;
        }
    }
    append(buf, RLP::encodeList(encoded));
}

void xtcash_trie_raw_full_node::EncodeRLP(xbytes_t & buf, std::error_code & ec) {
    xbytes_t encoded;
    encoded.reserve(1024);

    for (auto const & child : Children) {
        if (child == nullptr) {
            append(encoded, RLP::encode(nilValueNode.data()));  // 0x80 for empty bytes.
            continue;
        }

        switch (child->type()) {  // NOLINT(clang-diagnostic-switch-enum)
        case xtrie_node_type_t::hashnode: {
            assert(dynamic_cast<xtrie_hash_node_t *>(child.get()) != nullptr);
            append(encoded, RLP::encode(xspan_t<xbyte_t const>{std::dynamic_pointer_cast<xtrie_hash_node_t>(child)->data()}));

            break;
        }

        case xtrie_node_type_t::valuenode: {
            assert(dynamic_cast<xtrie_value_node_t *>(child.get()) != nullptr);
            append(encoded, RLP::encode(std::dynamic_pointer_cast<xtrie_value_node_t>(child)->data()));

            break;
        }

        case xtrie_node_type_t::fullnode: {
            assert(dynamic_cast<xtrie_full_node_t *>(child.get()) != nullptr);
            std::dynamic_pointer_cast<xtrie_full_node_t>(child)->EncodeRLP(encoded, ec);

            break;
        }

        case xtrie_node_type_t::shortnode: {
            assert(dynamic_cast<xtrie_short_node_t *>(child.get()) != nullptr);
            std::dynamic_pointer_cast<xtrie_short_node_t>(child)->EncodeRLP(encoded, ec);

            break;
        }

        case xtrie_node_type_t::rawshortnode: {
            assert(dynamic_cast<xtrie_raw_short_node_t *>(child.get()) != nullptr);
            std::dynamic_pointer_cast<xtrie_raw_short_node_t>(child)->EncodeRLP(encoded, ec);

            break;
        }

        default: {
            assert(false);  // NOLINT(clang-diagnostic-disabled-macro-expansion)
            xwarn("!!! raw full node not encode child type: %d", static_cast<uint8_t>(child->type()));

            break;
        }
        }
    }
    append(buf, RLP::encodeList(encoded));
}

void xtcash_trie_raw_short_node::EncodeRLP(xbytes_t & buf, std::error_code & ec) {
    xbytes_t encoded;
    encoded.reserve(1024);

    append(encoded, RLP::encode(Key));

    switch (Val->type()) {  // NOLINT(clang-diagnostic-switch-enum)
    case xtrie_node_type_t::hashnode: {
        assert(dynamic_cast<xtrie_hash_node_t *>(Val.get()) != nullptr);
        append(encoded, RLP::encode(xspan_t<xbyte_t const>{std::dynamic_pointer_cast<xtrie_hash_node_t>(Val)->data()}));

        break;
    }

    case xtrie_node_type_t::valuenode: {
        assert(dynamic_cast<xtrie_value_node_t *>(Val.get()) != nullptr);
        append(encoded, RLP::encode(std::dynamic_pointer_cast<xtrie_value_node_t>(Val)->data()));

        break;
    }

    case xtrie_node_type_t::fullnode: {
        assert(dynamic_cast<xtrie_full_node_t *>(Val.get()) != nullptr);
        std::dynamic_pointer_cast<xtrie_full_node_t>(Val)->EncodeRLP(encoded, ec);

        break;
    }

    case xtrie_node_type_t::rawfullnode: {
        assert(dynamic_cast<xtrie_raw_full_node_t *>(Val.get()) != nullptr);
        std::dynamic_pointer_cast<xtrie_raw_full_node_t>(Val)->EncodeRLP(encoded, ec);

        break;
    }

    default: {
        assert(false);  // NOLINT(clang-diagnostic-disabled-macro-expansion)
        xwarn("!!!! raw short node not encode type: %d", static_cast<uint8_t>(Val->type()));
        break;
    }
    }

    append(buf, RLP::encodeList(encoded));
}

NS_END3
