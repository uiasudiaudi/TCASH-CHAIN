// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xcxx_config.h"
#if defined(XCXX20)
#include "xkad/proto/ubuntu/kadmlia.pb.h"
#else
#include "xkad/proto/centos/kadmlia.pb.h"
#endif
#include "xkad/routing_table/elect_routing_table.h"
#include "xkad/routing_table/root_routing_table.h"
#include "xkad/routing_table/routing_utils.h"
#include "xpbase/base/tcash_config.h"
#include "xpbase/base/tcash_timer.h"
#include "xwrouter/multi_routing/routing_table_info_manager.h"
#include "xwrouter/root/root_routing.h"
#include "xwrouter/multi_routing/rrs_params_manager.h"

#include <map>
#include <memory>
#include <mutex>

namespace tcash {

namespace wrouter {

class RootRoutingManager;

class MultiRouting {
public:
    static MultiRouting * Instance();

    kadmlia::ElectRoutingTablePtr GetLastRoundRoutingTable(base::ServiceType const & service_type);
    kadmlia::ElectRoutingTablePtr GetElectRoutingTable(base::ServiceType const & service_type);
    kadmlia::RootRoutingTablePtr GetRootRoutingTable();
    void AddElectRoutingTable(base::ServiceType service_type, kadmlia::ElectRoutingTablePtr routing_table);
    void RemoveElectRoutingTable(base::ServiceType service_type);

    void GetAllRegisterType(std::vector<base::ServiceType> & vec_type);
    void GetAllRegisterRoutingTable(std::vector<std::shared_ptr<kadmlia::ElectRoutingTable>> & vec_rt);

    void CheckElectRoutingTable(base::ServiceType service_type);

public:
    int CreateRootRouting(std::shared_ptr<transport::Transport> transport, const base::Config & config, base::KadmliaKeyPtr kad_key_ptr);

public:
    void HandleRootMessage(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);

    void HandleCacheElectNodesRequest(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);
    void HandleCacheElectNodesResponse(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);

    void SendAddLastRoundElectNodesRequest(kadmlia::ElectRoutingTablePtr elect_routing_table_ptr, base::ServiceType service_type);
    void HandleAddLastRoundElectNodesRequest(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);
    void HandleAddLastRoundElectNodesResponse(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);

    // RoutingTableInfoMgr
    void add_routing_table_info(common::xip2_t const & group_xip, uint64_t routing_table_blk_height);
    void delete_routing_table_info(common::xip2_t const & group_xip, uint64_t version_or_blk_height);
    // base::ServiceType transform_service_type(base::ServiceType const & service_type);
    // std::vector<kadmlia::NodeInfoPtr> transform_node_vec(base::ServiceType const & service_type, std::vector<kadmlia::NodeInfoPtr> const & node_vec);

    // Update RRS N
    bool UpdateNodeSizeCallback(std::function<void(uint64_t & node_size, std::error_code & ec)> cb);

private:
    std::shared_ptr<wrouter::RootRouting> root_routing_table_;
    std::mutex root_routing_table_mutex_;

    MultiRouting();
    ~MultiRouting();

    std::map<base::ServiceType, kadmlia::ElectRoutingTablePtr> elect_routing_table_map_;
    std::mutex elect_routing_table_map_mutex_;

    RoutingTableInfoMgr routing_table_info_mgr;

    std::shared_ptr<RRSParamsMgr> rrs_params_mgr_ptr;

    base::TimerManager * timer_manager_{base::TimerManager::Instance()};
    std::shared_ptr<base::TimerRepeated> check_elect_routing_;
    void CheckElectRoutingTableTimer();

    using OnCompleteElectRoutingTableCallback = std::function<void(base::ServiceType const, std::string const, kadmlia::NodeInfoPtr const &)>;
    void CompleteElectRoutingTable();
    void OnCompleteElectRoutingTable(base::ServiceType const service_type, std::string const election_xip2, kadmlia::NodeInfoPtr const & node_info);

    void OnAddLastRoundElectRoutingNodes(int status, transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);

    DISALLOW_COPY_AND_ASSIGN(MultiRouting);
};

}  // namespace wrouter

}  // namespace tcash
