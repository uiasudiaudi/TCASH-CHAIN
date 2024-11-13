// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xspan.h"
#include "xcommon/rlp.h"
#include "xevm_common/trie/xtrie_node.h"

NS_BEG3(tcash, evm_common, trie)

class xtcash_trie_node_rlp {
public:
    static xbytes_t EncodeToBytes(xtrie_node_face_ptr_t const & node);

    template <std::size_t len>
    static xbytes_t EncodeNodesToBytes(std::array<xtrie_node_face_ptr_t, len> nodes);

    //static xtrie_node_face_ptr_t decodeNode(xh256_t const & hash_bytes, xbytes_t const & buf, std::error_code & ec);
    //static xtrie_node_face_ptr_t decodeNode(std::shared_ptr<xtrie_hash_node_t> hash, xbytes_t const & buf, std::error_code & ec);
    //static xtrie_node_face_ptr_t mustDecodeNode(xh256_t const & hash_bytes, xbytes_t const & buf);

    static xtrie_node_face_ptr_t decode_node(xh256_t const & hash_bytes, xbytes_t const & buf, std::error_code & ec);
    static xtrie_node_face_ptr_t decode_node(xh256_t const & hash_bytes, xspan_t<xbyte_t const> buf, std::error_code & ec);
    static xtrie_node_face_ptr_t decode_node(std::shared_ptr<xtrie_hash_node_t> hash, xspan_t<xbyte_t const>  buf, std::error_code & ec);
    static xtrie_node_face_ptr_t must_decode_node(xh256_t const & hash_bytes, xbytes_t const & buf);
    static xtrie_node_face_ptr_t must_decode_node(xh256_t const & hash_bytes, xspan_t<xbyte_t const> buf);

private:
    //static xtrie_node_face_ptr_t decodeShort(std::shared_ptr<xtrie_hash_node_t> hash, xbytes_t const & elems, std::error_code & ec);
    //static xtrie_node_face_ptr_t decodeFull(std::shared_ptr<xtrie_hash_node_t> hash, xbytes_t const & elems, std::error_code & ec);
    //static std::pair<xtrie_node_face_ptr_t, xbytes_t> decodeRef(xbytes_t const & buf, std::error_code & ec);

    static xtrie_node_face_ptr_t decode_short(std::shared_ptr<xtrie_hash_node_t> hash, xspan_t<xbyte_t const> elems, std::error_code & ec);
    static xtrie_node_face_ptr_t decode_full(std::shared_ptr<xtrie_hash_node_t> hash, xspan_t<xbyte_t const> elems, std::error_code & ec);
    static std::pair<xtrie_node_face_ptr_t, xspan_t<xbyte_t const>> decode_ref(xspan_t<xbyte_t const> buf, std::error_code & ec);
};
using xtrie_node_rlp = xtcash_trie_node_rlp;

NS_END3
