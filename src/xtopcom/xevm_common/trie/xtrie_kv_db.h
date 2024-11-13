// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xnode_id.h"
#include "xevm_common/trie/xtrie_kv_db_face.h"
#include "xvledger/xvdbstore.h"

namespace tcash {
namespace evm_common {
namespace trie {

class xtcash_kv_db : public xkv_db_face_t {
private:
    base::xvdbstore_t * m_db{nullptr};
    mutable std::mutex m_mutex;
    common::xtable_address_t m_table;  // store key with table address
    std::string m_node_key_prefix;

    std::string convert_key(xspan_t<xbyte_t const> key) const;

public:
    xtcash_kv_db(xtcash_kv_db const &) = delete;
    xtcash_kv_db & operator=(xtcash_kv_db const &) = delete;
    xtcash_kv_db(xtcash_kv_db &&) = delete;
    xtcash_kv_db & operator=(xtcash_kv_db &&) = delete;
    ~xtcash_kv_db() override = default;

    xtcash_kv_db(base::xvdbstore_t * db, common::xtable_address_t table);

    xbytes_t get(xspan_t<xbyte_t const>  key, std::error_code & ec) const override;
    xbytes_t GetDirect(xbytes_t const & key, std::error_code & ec) const override;

    bool has(xspan_t<xbyte_t const> key, std::error_code & ec) const override;
    bool HasDirect(xbytes_t const & key, std::error_code & ec) const override;

    void Put(xspan_t<xbyte_t const> key, xbytes_t const & value, std::error_code & ec) override;
    void PutBatch(std::map<xh256_t, xbytes_t> const & batch, std::error_code & ec) override;
    void PutDirect(xbytes_t const & key, xbytes_t const & value, std::error_code & ec) override;
    void PutDirectBatch(std::map<xbytes_t, xbytes_t> const & batch, std::error_code & ec) override;

    void Delete(xbytes_t const & key, std::error_code & ec) override;
    void DeleteBatch(std::vector<xspan_t<xbyte_t const>> const & batch, std::error_code & ec) override;
    void DeleteDirect(xbytes_t const & key, std::error_code & ec) override;
    void DeleteDirectBatch(std::vector<xbytes_t> const & batch, std::error_code & ec) override;

    common::xtable_address_t table_address() const override;
};
using xkv_db_t = xtcash_kv_db;

}  // namespace trie
}  // namespace state_mpt
}  // namespace tcash
