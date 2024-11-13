// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xaccount_address.h"
#include "xvnetwork/xvnetwork_driver_face.h"

namespace tcash {
namespace state_sync {

struct sync_result {
    common::xaccount_address_t account;
    uint64_t height{0};
    xh256_t block_hash;
    xh256_t state_hash;
    xh256_t root_hash;
    std::error_code ec;

    sync_result(common::xaccount_address_t _account, uint64_t h, xh256_t _block_hash, xh256_t _state_hash, xh256_t _root, std::error_code _ec)
      : account(_account), height(h), block_hash(_block_hash), state_hash(_state_hash), root_hash(_root), ec(_ec) {
    }
    sync_result() = default;
};

struct sync_peers {
    std::shared_ptr<vnetwork::xvnetwork_driver_face_t> network;
    std::vector<vnetwork::xvnode_address_t> peers;
};

enum class state_req_type {
    enum_state_req_table,
    enum_state_req_trie,
    enum_state_req_unit,
};

struct state_req {
    uint32_t id;
    state_req_type type;
    common::xnode_address_t peer;
    uint32_t n_items{0};
    std::set<xh256_t> trie_tasks;
    std::map<xh256_t, xbytes_t> unit_tasks;
    uint64_t start{0};
    uint64_t delivered{0};
    std::vector<xbytes_t> nodes_response;
    std::vector<xbytes_t> units_response;
};

struct state_res {
    uint32_t id;
    std::vector<xbytes_t> nodes;
    std::vector<xbytes_t> units;
};

class xtcash_state_sync_face {
public:
    xtcash_state_sync_face() = default;
    virtual ~xtcash_state_sync_face() = default;

    virtual void run() = 0;
    virtual void cancel() = 0;
    virtual bool is_done() const = 0;
    virtual std::error_code error() const = 0;
    virtual std::string symbol() const = 0;
    virtual sync_result result() const = 0;
    virtual void deliver_req(const state_req & req) = 0;
};
using xstate_sync_face_t = xtcash_state_sync_face;

}  // namespace state_sync
}  // namespace tcash