#include "xelect_net/include/elect_manager.h"

#include "xbasic/xelapsed_time.h"
#include "xbasic/xutility.h"
#include "xchain_timer/xchain_timer.h"
#include "xdata/xelection/xelection_cluster_result.h"
#include "xdata/xelection/xelection_group_result.h"
#include "xdata/xelection/xelection_info.h"
#include "xdata/xelection/xelection_info_bundle.h"
#include "xdata/xelection/xelection_network_result.h"
#include "xdata/xelection/xelection_result.h"
#include "xelect_net/include/elect_uitils.h"
#include "xpbase/base/tcash_log.h"
#include "xwrouter/multi_routing/small_net_cache.h"
#include "xwrouter/register_message_handler.h"

#include <chrono>
#include <cinttypes>
#include <limits>

namespace tcash {

namespace elect {

ElectManager::ElectManager(transport::TransportPtr transport, const base::Config & config) : transport_(transport), config_(config) {
}

void ElectManager::OnElectUpdated(const data::election::xelection_result_store_t & election_result_store,
                                  common::xzone_id_t const & zid,
                                  std::uint64_t const associated_blk_height) {
    using tcash::data::election::xelection_cluster_result_t;
    using tcash::data::election::xelection_group_result_t;
    using tcash::data::election::xelection_info_bundle_t;
    using tcash::data::election::xelection_info_t;
    using tcash::data::election::xelection_result_store_t;
    using tcash::data::election::xelection_result_t;

    for (auto const & election_result_info : election_result_store) {
        auto const network_id = tcash::get<common::xnetwork_id_t const>(election_result_info);
        auto const & election_type_results = tcash::get<data::election::xelection_network_result_t>(election_result_info);
        for (auto const & election_type_result : election_type_results) {
            // auto node_type = tcash::get<common::xnode_type_t const>(election_type_result);
            auto const & election_result = tcash::get<data::election::xelection_result_t>(election_type_result);

            for (auto const & cluster_result_info : election_result) {
                auto const & cluster_id = tcash::get<common::xcluster_id_t const>(cluster_result_info);
                auto const & cluster_result = tcash::get<xelection_cluster_result_t>(cluster_result_info);

                for (auto const & group_result_info : cluster_result) {
                    auto const & group_id = tcash::get<common::xgroup_id_t const>(group_result_info);
                    auto const & group_result = tcash::get<xelection_group_result_t>(group_result_info);

                    common::xip2_t group_xip{network_id, zid, cluster_id, group_id};

                    auto const size = static_cast<uint16_t>(group_result.size());
                    // auto const & version_height = group_result.group_version().value();  // use group_version as xip height;
                    auto const blk_height = associated_blk_height;                     // use blk_height as xip height;

                    std::vector<wrouter::WrouterTableNode> elect_data;
                    for (auto const & node_info : group_result) {
                        auto const & node_id = tcash::get<xelection_info_bundle_t>(node_info).account_address();
                        // auto const & election_info = tcash::get<xelection_info_bundle_t>(node_info).election_info();
                        auto const & slot_id = tcash::get<const common::xslot_id_t>(node_info);
                        // here the slot_id is strict increasing. Start with 0.
                        if (node_id.empty()) {
                            continue;
                        }

                        common::xip2_t xip2_{network_id,
                                             zid,
                                             cluster_id,
                                             group_id,
                                             slot_id,
                                             size,
                                             blk_height};

                        wrouter::WrouterTableNode router_node{xip2_, node_id.to_string()};
                        xinfo("[ElectManager::OnElectUpdated] %s %s", xip2_.to_string().c_str(), node_id.to_string().c_str());

                        // ElectNetNode enode{node_id.to_string(), election_info.consensus_public_key.to_string(), xip, "", associated_gid, version};
                        elect_data.push_back(router_node);
                    }
                    OnElectUpdated(elect_data, group_xip, blk_height);
                }
            }
        }
    }
}

void ElectManager::OnElectUpdated(std::vector<wrouter::WrouterTableNode> const & elect_data,
                                  common::xip2_t const & group_xip,
                                  uint64_t const routing_table_blk_height) {
    auto const service_type = base::ServiceType::build_from(group_xip, routing_table_blk_height);
    xinfo("[ElectManager::OnElectUpdated] service type %" PRIx64 " %s", service_type.value(), service_type.info().c_str());
    wrouter::SmallNetNodes::Instance()->AddNode(elect_data);
    wrouter::MultiRouting::Instance()->add_routing_table_info(group_xip, routing_table_blk_height);
    for (auto const & wrouter_node : elect_data) {
        if (global_node_id != wrouter_node.node_id) {
            xdbg("node id not match self:%s iter:%s", global_node_id.c_str(), wrouter_node.node_id.c_str());
            continue;
        }
        xinfo("account match: self:%s iter:%s ,xip:%s ", global_node_id.c_str(), wrouter_node.node_id.c_str(), wrouter_node.m_xip2.to_string().c_str());
        UpdateRoutingTable(elect_data, wrouter_node);
        // break;
    }
}

void ElectManager::UpdateRoutingTable(std::vector<wrouter::WrouterTableNode> const & elect_data, wrouter::WrouterTableNode const & self_wrouter_nodes) {
    base::KadmliaKeyPtr kad_key = base::GetKadmliaKey(self_wrouter_nodes.m_xip2);
    base::ServiceType service_type = kad_key->GetServiceType();

    assert(service_type.IsBroadcastService() == false); // use blk_height , dont have broadcast height any more.
    // if (service_type.IsBroadcastService()) {
    //     wrouter::MultiRouting::Instance()->RemoveElectRoutingTable(service_type);
    // } else 
    if (wrouter::MultiRouting::Instance()->GetElectRoutingTable(service_type) != nullptr) {
        xinfo("ElectManager::UpdateRoutingTable get repeated routing table info xip2: service_type:%s", self_wrouter_nodes.m_xip2.to_string().c_str(), service_type.info().c_str());
        return;
    }

    base::Config config = config_;
    if (!config.Set("node", "network_id", self_wrouter_nodes.m_xip2.network_id().value())) {
        tcash_ERROR("set config node network_id [%d] failed!", self_wrouter_nodes.m_xip2.network_id().value());
        return;
    }

    kadmlia::LocalNodeInfoPtr local_node_ptr = kadmlia::CreateLocalInfoFromConfig(config, kad_key);
    if (!local_node_ptr) {
        tcash_WARN("local_node_ptr invalid");
        return;
    }

    auto routing_table_ptr = std::make_shared<kadmlia::ElectRoutingTable>(transport_, local_node_ptr);
    if (!routing_table_ptr->Init()) {
        xerror("init election routing table failed!");
        return;
    }

    auto const root_routing = wrouter::MultiRouting::Instance()->GetRootRoutingTable();

    std::map<std::string, base::KadmliaKeyPtr> elect_root_kad_key_ptrs;
    std::set<std::string> new_round_root_xip_set;
    for (auto const & node : elect_data) {
        auto root_kad_key = base::GetRootKadmliaKey(node.node_id);
        elect_root_kad_key_ptrs.insert(std::make_pair(node.m_xip2.to_string(), root_kad_key));
        new_round_root_xip_set.insert(root_kad_key->Get());
    }

    std::map<std::string, kadmlia::NodeInfoPtr> last_round_out_nodes_map;

    // lastest election_nodes
    auto const last_round_routing_table_ptr = wrouter::MultiRouting::Instance()->GetLastRoundRoutingTable(service_type);
    if (last_round_routing_table_ptr) {
        xdbg("ElectManager::UpdateRoutingTable find last round routing_table,service_type: %s this_round:%s",
             last_round_routing_table_ptr->get_local_node_info()->service_type().info().c_str(),
             service_type.info().c_str());
        auto last_round_nodes_map = last_round_routing_table_ptr->GetAllNodesRootKeyMap();
        for (auto const & p : last_round_nodes_map) {
            auto const & last_election_xip2 = tcash::get<std::string const>(p);
            auto const root_kad_key = tcash::get<base::KadmliaKeyPtr>(p);
            auto const & root_xip = root_kad_key->Get();
            if (new_round_root_xip_set.find(root_xip) == new_round_root_xip_set.end()) {
                // auto const & node_info = last_round_routing_table_ptr->GetNode(last_election_xip2);
                // here is possible to be nullptr (ths dst node never connected in this p2p)
                // but set it anyway, must be sure the index is same in every correct nodes
                // since the dst node(elected out) wasn't even be online in the whole last round,
                // make sure won't core dump while try send packet.
                // assert(node_info != nullptr);
                last_round_out_nodes_map.insert({last_election_xip2, last_round_routing_table_ptr->GetNode(last_election_xip2)});
            }
        }
    }
    if (last_round_out_nodes_map.empty() && service_type.height()) {
        routing_table_ptr->set_lack_last_round_nodes(true);
    }

    routing_table_ptr->SetElectionNodesExpected(elect_root_kad_key_ptrs, last_round_out_nodes_map);
    local_node_ptr->set_public_ip(root_routing->get_local_node_info()->public_ip());
    local_node_ptr->set_public_port(root_routing->get_local_node_info()->public_port());

    wrouter::MultiRouting::Instance()->AddElectRoutingTable(service_type, std::move(routing_table_ptr));
    wrouter::MultiRouting::Instance()->CheckElectRoutingTable(service_type);
    return;
}

void ElectManager::OnElectQuit(const common::xip2_t & xip2) {
    auto service_type = base::GetKadmliaKey(xip2)->GetServiceType();
    wrouter::MultiRouting::Instance()->delete_routing_table_info(service_type.group_xip2(), service_type.height());
    wrouter::MultiRouting::Instance()->RemoveElectRoutingTable(service_type);
    xdbg("OnElectQuit service_type:%s xip2:%s", service_type.info().c_str(), xip2.to_string().c_str());
}

}  // namespace elect

}  // namespace tcash
