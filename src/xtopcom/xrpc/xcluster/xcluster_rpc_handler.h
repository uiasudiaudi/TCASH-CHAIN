﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "xrouter/xrouter.h"
#include "xrouter/xrouter_face.h"
#include "xrpc/xrpc_define.h"
#include "xrpc/xrpc_msg_define.h"
#include "xrpc/xrule_manager.h"
#include "xtxpool_service_v2/xtxpool_service_face.h"
#include "xvnetwork/xaddress.h"
#include "xvnetwork/xmessage.h"
#include "xvnetwork/xvhost_face.h"
#include "xvledger/xvtxstore.h"

NS_BEG2(tcash, xrpc)
using router::xrouter_face_t;
using vnetwork::xmessage_t;
using vnetwork::xvnetwork_driver_face_t;
using vnetwork::xvnode_address_t;

class xcluster_rpc_handler : public std::enable_shared_from_this<xcluster_rpc_handler> {
public:
    xcluster_rpc_handler(std::shared_ptr<xvnetwork_driver_face_t>        cluster_vhost,
                         observer_ptr<xrouter_face_t>                    router_ptr,
                         xtxpool_service_v2::xtxpool_proxy_face_ptr const & txpool_service,
                         observer_ptr<base::xvblockstore_t>              block_store,
                         observer_ptr<base::xvtxstore_t>                 txstore,
                         observer_ptr<tcash::base::xiothread_t>            thread);
    void on_message(const xvnode_address_t & edge_sender, xmessage_t const & message);
    void cluster_process_request(const xrpc_msg_request_t & edge_msg, const xvnode_address_t & edge_sender, const xmessage_t & message);
    void cluster_process_response(const xmessage_t & msg, const xvnode_address_t & shard_sender);
    void start();
    void stcash();

private:
    std::shared_ptr<vnetwork::xvnetwork_driver_face_t>   m_cluster_vhost;
    observer_ptr<xrouter_face_t>                         m_router_ptr;
    xtxpool_service_v2::xtxpool_proxy_face_ptr           m_txpool_service;
    unique_ptr<xfilter_manager>                          m_rule_mgr_ptr;
    observer_ptr<tcash::base::xiothread_t>                 m_thread;
};

NS_END2
