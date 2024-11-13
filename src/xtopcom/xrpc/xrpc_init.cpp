// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xrpc_init.h"
#include "xcommon/xnode_type.h"
#include "xrpc/xhttp/xevm_server.h"
#include "xrpc/xhttp/xhttp_server.h"
// #include "xrpc/xws/xws_server.h"

NS_BEG2(tcash, xrpc)

tcash::base::xiothread_t* xrpc_init::m_thread = nullptr;

xrpc_init::xrpc_init(std::shared_ptr<xvnetwork_driver_face_t> vhost,
              const common::xnode_type_t node_type,
              observer_ptr<xrouter_face_t> router_ptr,
              const uint16_t http_port,
              const uint16_t ws_port,
              xtxpool_service_v2::xtxpool_proxy_face_ptr const &  txpool_service,
              observer_ptr<base::xvblockstore_t> const & block_store,
              observer_ptr<base::xvtxstore_t> const & txstore,
              observer_ptr<elect::ElectMain> elect_main,
              observer_ptr<tcash::election::cache::xdata_accessor_face_t> const & election_cache_data_accessor)
{
    assert(nullptr != vhost);
    assert(nullptr != router_ptr);
    switch (node_type) {
    case common::xnode_type_t::consensus_validator:
    case common::xnode_type_t::evm_validator:
        assert(nullptr != txpool_service);
        init_rpc_cb_thread();
        m_shard_handler = std::make_shared<xshard_rpc_handler>(vhost, txpool_service, make_observer(m_thread));
        m_shard_handler->start();
        break;
    case common::xnode_type_t::committee:
        XATTRIBUTE_FALLTHROUGH;
    case common::xnode_type_t::zec:
        XATTRIBUTE_FALLTHROUGH;
    case common::xnode_type_t::consensus_auditor:
        XATTRIBUTE_FALLTHROUGH;
    case common::xnode_type_t::evm_auditor:
        assert(nullptr != txpool_service);
        init_rpc_cb_thread();
        m_cluster_handler = std::make_shared<xcluster_rpc_handler>(vhost, router_ptr, txpool_service, block_store, txstore, make_observer(m_thread));
        m_cluster_handler->start();
        break;
    case common::xnode_type_t::edge: {
        init_rpc_cb_thread();
        m_edge_handler = std::make_shared<xrpc_edge_vhost>(vhost, router_ptr, make_observer(m_thread));
        auto ip = vhost->address().xip2();

        xdbg("edge http");
        shared_ptr<xhttp_server> http_server_ptr = std::make_shared<xhttp_server>(m_edge_handler, ip, false, block_store, txstore, elect_main, election_cache_data_accessor);
        http_server_ptr->start(http_port);
        // websocket is no use now.
        // shared_ptr<xws_server> ws_server_ptr = std::make_shared<xws_server>(m_edge_handler, ip, false, block_store, txstore, elect_main, election_cache_data_accessor);
        // ws_server_ptr->start(ws_port);

        xdbg("edge evm");
        shared_ptr<xevm_server> evm_server_ptr = std::make_shared<xevm_server>(m_edge_handler, ip, false, block_store, txstore, elect_main, election_cache_data_accessor);
        evm_server_ptr->start(XGET_CONFIG(evm_port));
        break;
    }
    case common::xnode_type_t::storage_archive:
    {
        xdbg("arc rpc start");
        init_rpc_cb_thread();
        m_rpc_handler = std::make_shared<xrpc_handler>(vhost, router_ptr, txpool_service, block_store, txstore, make_observer(m_thread));
        m_rpc_handler->start();
        break;
    }
    case common::xnode_type_t::storage_exchange:
    {
        init_rpc_cb_thread();
        m_edge_handler = std::make_shared<xrpc_edge_vhost>(vhost, router_ptr, make_observer(m_thread));
        auto ip = vhost->address().xip2();
        shared_ptr<xhttp_server> http_server_ptr = std::make_shared<xhttp_server>(m_edge_handler, ip, true, block_store, txstore, elect_main, election_cache_data_accessor);
        http_server_ptr->start(http_port);
        // websocket is no use now.
        // shared_ptr<xws_server> ws_server_ptr = std::make_shared<xws_server>(m_edge_handler, ip, true, block_store, txstore, elect_main, election_cache_data_accessor);
        // ws_server_ptr->start(ws_port);
        xdbg("start exchange rpc service.");

        // XTODO evm rpc server not enabled in exchange node
        // shared_ptr<xevm_server> evm_server_ptr = std::make_shared<xevm_server>(m_edge_handler, ip, true, block_store, txstore, elect_main, election_cache_data_accessor);
        // evm_server_ptr->start(XGET_CONFIG(evm_port));
        break;
    }
    case common::xnode_type_t::fullnode: {
        xdbg("fullnode rpc start");
        init_rpc_cb_thread();
        m_rpc_handler = std::make_shared<xrpc_handler>(vhost, router_ptr, txpool_service, block_store, txstore, make_observer(m_thread));
        m_rpc_handler->start();

        break;
    }

    case common::xnode_type_t::relay: {
        xdbg("relay rpc start");
        init_rpc_cb_thread();

        xdbg("relay evm");
        m_edge_handler = std::make_shared<xrpc_edge_vhost>(vhost, router_ptr, make_observer(m_thread));
//        auto ip = vhost->address().xip2();
//        shared_ptr<xevm_server> evm_server_ptr = std::make_shared<xevm_server>(m_edge_handler, ip, false, block_store, txstore, elect_main, election_cache_data_accessor);
//        evm_server_ptr->start(XGET_CONFIG(evm_port));

        break;
    }

    default:
        assert(false);
        break;
    }
}

void xrpc_init::stcash() {
    if (m_shard_handler != nullptr) {
        m_shard_handler->stcash();
    }

    if (m_cluster_handler != nullptr) {
        m_cluster_handler->stcash();
    }

    if (m_rpc_handler != nullptr) {
        m_rpc_handler->stcash();
    }
}

void xrpc_init::init_rpc_cb_thread(){
    if(m_thread == nullptr){
        m_thread = base::xiothread_t::create_thread(base::xcontext_t::instance(), 0, -1);
    }
}

NS_END2
