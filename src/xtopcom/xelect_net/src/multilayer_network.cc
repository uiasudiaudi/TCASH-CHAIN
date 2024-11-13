// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "xelect_net/include/multilayer_network.h"

#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdb/xdb_factory.h"
#include "xelect_net/include/http_seed_fetcher.h"
#include "xelect_net/include/https_seed_fetcher.h"
#include "xkad/routing_table/routing_utils.h"
#include "xtransport/udp_transport/udp_transport.h"
#include "xwrouter/multi_routing/multi_routing.h"
#include "xwrouter/multi_routing/service_node_cache.h"
#include "xwrouter/multi_routing/small_net_cache.h"

#include <array>
#include <cstdlib>
#include <ctime>

#include <unistd.h>

namespace tcash {
namespace elect {

static const std::string kKadmliaKeyDbKey = "KADMLIA_KEY_DB_KEY";
static const std::string kKadmliaKeyField("kad_key_");
static const std::string kConfigFile("config/config.json");

MultilayerNetwork::MultilayerNetwork(common::xnode_id_t const & node_id, const std::set<uint32_t> & xnetwork_id_set) : m_node_id_{node_id}, xnetwork_id_set_{xnetwork_id_set} {
    ec_netcard_ = std::make_shared<EcNetcard>();
    xinfo("create ec_netcard");
    multi_message_handler_ = std::make_shared<tcash::transport::MultiThreadHandler>();
    xinfo("create multi_message_handler");
    core_transport_ = std::make_shared<tcash::transport::UdpTransport>();
    xinfo("create core_transport");

    for (const auto & xnetwork_id : xnetwork_id_set_) {
        auto ecvhost = std::make_shared<EcVHost>(xnetwork_id, GetEcNetcard(), m_node_id_);
        {
            std::unique_lock<std::mutex> lock(vhost_map_mutex_);
            vhost_map_[xnetwork_id] = ecvhost;
        }
        xinfo("add ecvhost xnetwork_id:%u to map", xnetwork_id);
    }
}

void MultilayerNetwork::start() {
    tcash::base::Config config;
    TryCombineP2PEndpoints();

    if (!Init(config)) {
        xerror("start elect main init failed!");
        throw std::runtime_error{"start elect main init failed!"};
    }

    elect_manager_ = std::make_shared<ElectManager>(GetCoreTransport(), config);

    if (!elect_manager_) {
        xerror("elect_manager create failed");
        throw std::runtime_error{"elect_manager init failed"};
    }

    if (!Run(config)) {
        xerror("elect main start failed");
        throw std::runtime_error{"elect main start failed"};
    }
}

void MultilayerNetwork::stcash() {
    for (const auto & xnetwork_id : xnetwork_id_set_) {
        auto ecvhost = GetEcVhost(xnetwork_id);
        if (!ecvhost) {
            continue;
        }
        ecvhost->stcash();
    }

    elect_manager_.reset();

    // root_manager_ptr_.reset();
    if (core_transport_) {
        core_transport_->Stcash();
        core_transport_.reset();
    }

    if (nat_transport_) {
        nat_transport_->Stcash();
        nat_transport_.reset();
    }

    multi_message_handler_.reset();
}

bool MultilayerNetwork::Init(const base::Config & config) {
    if (!tcash::kadmlia::CreateGlobalXid(config)) {
        return false;
    }
    tcash::kadmlia::CallbackManager::Instance();

    if (!multi_message_handler_) {
        xerror("multi_message_handler empty");
        return false;
    }
    multi_message_handler_->Init();

    std::string local_ip = XGET_CONFIG(ip);
    uint16_t local_port = XGET_CONFIG(node_p2p_port);
    if (!core_transport_->Init(local_ip, local_port, multi_message_handler_.get())) {
        xerror("MultilayerNetwork::Init udptransport init failed");
        return false;
    }

    wrouter::SmallNetNodes::Instance()->Init();
    xinfo("Init SmallNetNodes for Elect Network");
    wrouter::ServiceNodes::Instance()->Init();
    xinfo("Init ServiceNodes for cache nodes");

    core_transport_->register_on_receive_callback(std::bind(&wrouter::Wrouter::recv, wrouter::Wrouter::Instance(), std::placeholders::_1, std::placeholders::_2));
    // attention: Wrouter::Instance()->Init must put befor core_transport->Start
    wrouter::Wrouter::Instance()->Init(core_transport_);

    if (core_transport_->Start() != tcash::kadmlia::kKadSuccess) {
        xerror("start local udp transport failed!");
        return false;
    }
    core_transport_->RegisterOfflineCallback(kadmlia::HeartbeatManagerIntf::OnHeartbeatCallback);

    if (!ec_netcard_) {
        xerror("ec_netcard creat failed");
        return false;
    }
    xinfo("created EcNetcard");
    ec_netcard_->Init();
    // elect_cmd_.set_netcard(ec_netcard_);
    return true;
}

bool MultilayerNetwork::Run(const base::Config & config) {
    if (ResetRootRouting(GetCoreTransport(), config) == tcash::kadmlia::kKadFailed) {
        xwarn("kRoot join network failed");
        return false;
    }

    for (const auto & xnetwork_id : xnetwork_id_set_) {
        auto ecvhost = GetEcVhost(xnetwork_id);
        if (!ecvhost) {
            continue;
        }
        ecvhost->start();
    }

    return true;
}

bool MultilayerNetwork::RegisterNodeCallback(std::function<int32_t(std::string const & node_addr, std::string const & node_sign)> cb) {
    if (GetCoreTransport() == nullptr) {
        xwarn("register node callback fail: core_transport is nullptr");
        return false;
    }

    GetCoreTransport()->RegisterNodeCallback(cb);
    xinfo("register node callback successful");
    return true;
}

bool MultilayerNetwork::UpdateNodeSizeCallback(std::function<void(uint64_t & node_size, std::error_code & ec)> cb) {
    return wrouter::MultiRouting::Instance()->UpdateNodeSizeCallback(cb);
}

std::shared_ptr<elect::xnetwork_driver_face_t> MultilayerNetwork::GetEcVhost(const uint32_t & xnetwork_id) const noexcept {
    std::unique_lock<std::mutex> lock(vhost_map_mutex_);
    auto ifind = vhost_map_.find(xnetwork_id);
    if (ifind == vhost_map_.end()) {
        return nullptr;
    }
    return std::static_pointer_cast<elect::xnetwork_driver_face_t>(ifind->second);
}

void MultilayerNetwork::TryCombineP2PEndpoints() {
    std::string settled_p2p_endpoints = XGET_CONFIG(p2p_endpoints);
    xinfo("config get settled_p2p_endpoints %s", settled_p2p_endpoints.c_str());

    std::string combined_p2p_endpoints = settled_p2p_endpoints;
    std::string url_endpoints = XGET_CONFIG(p2p_url_endpoints);
    xinfo("[seeds] url_endpoints url:%s", url_endpoints.c_str());
    if (url_endpoints.find("http") == std::string::npos) {
        // url not support nothing todo.
        return;
    }

    // fetch using http
    std::vector<std::string> url_seeds;
    if (url_endpoints.find("https://") != std::string::npos) {
        HttpsSeedFetcher seed_fetcher{url_endpoints};
        if (!seed_fetcher.GetSeeds(url_seeds)) {
            xwarn("[seeds] get public endpoints failed from url:%s", url_endpoints.c_str());
            return;
        }
    } else {  // http scheme
        HttpSeedFetcher seed_fetcher{url_endpoints};
        if (!seed_fetcher.GetSeeds(url_seeds)) {
            xwarn("[seeds] get public endpoints failed from url:%s", url_endpoints.c_str());
            return;
        }
    }

    static std::array<std::string, 2> seed_delimiter{",", ""};
    // combine url's endpoints together.
    for (const auto & item : url_seeds) {
        combined_p2p_endpoints += seed_delimiter[static_cast<size_t>(combined_p2p_endpoints.empty())];
        combined_p2p_endpoints += item;
    }
    //if (!combined_p2p_endpoints.empty() && combined_p2p_endpoints.back() == ',') {
    //    combined_p2p_endpoints.pop_back();
    //}
    xinfo("config get combined_p2p_endpoints %s", combined_p2p_endpoints.c_str());

    XSET_CONFIG(p2p_endpoints, combined_p2p_endpoints);
}

int MultilayerNetwork::CreateRootManager(std::shared_ptr<transport::Transport> transport,
                                         const tcash::base::Config & config,
                                         const std::set<std::pair<std::string, uint16_t>> & p2p_endpoints_config) {
    xinfo("enter CreateRootManager");
    tcash::base::Config new_config = config;
    if (!new_config.Set("edge", "service_list", "")) {
        xerror("set config edge service_list failed!");
        return tcash::kadmlia::kKadFailed;
    }

    // uint32_t zone_id = 0;
    // if (!kadmlia::GetZoneIdFromConfig(config, zone_id)) {
    //     xerror("get zone id from config failed!");
    //     return tcash::kadmlia::kKadFailed;
    // }

    // get kroot id
    assert(!global_node_id.empty());
    base::KadmliaKeyPtr kad_key_ptr = base::GetRootKadmliaKey(global_node_id);
    xinfo("get root kad key: %s", kad_key_ptr->Get().c_str());

    if (wrouter::MultiRouting::Instance()->CreateRootRouting(transport, new_config, kad_key_ptr) != tcash::kadmlia::kKadSuccess) {
        // if (root_manager_ptr->InitRootRoutingTable(transport, new_config, kad_key_ptr) != tcash::kadmlia::kKadSuccess) {
        xerror("<blueshi> add root_table[root] failed!");
        return tcash::kadmlia::kKadFailed;
    }
    xinfo("CreateRootManager ok.");
    return tcash::kadmlia::kKadSuccess;
}

int MultilayerNetwork::ResetRootRouting(std::shared_ptr<transport::Transport> transport, const base::Config & config) {
    std::set<std::pair<std::string, uint16_t>> p2p_endpoints_config;
    kadmlia::GetPublicEndpointsConfig(config, p2p_endpoints_config);

    return CreateRootManager(transport, config, p2p_endpoints_config);
}

std::vector<std::string> MultilayerNetwork::GetServiceNeighbours(const common::xip2_t & xip2) {
    std::vector<std::string> nodes_vec;

    auto kad_key = base::GetKadmliaKey(xip2);
    auto rt = wrouter::MultiRouting::Instance()->GetElectRoutingTable(kad_key->GetServiceType());
    if (!rt) {
        xwarn("no routinng table:%llu registered, GetServiceNeighbours failed", kad_key->GetServiceType().value());
        return {};
    }

    std::vector<kadmlia::NodeInfoPtr> vec;
    rt->GetRandomNodes(vec, rt->nodes_size());
    for (const auto & item : vec) {
        auto ifind = std::find(nodes_vec.begin(), nodes_vec.end(), item->public_ip);
        if (ifind == nodes_vec.end()) {
            nodes_vec.push_back(item->public_ip);
            xdbg("GetServiceNeighbours %llu add node:%s", kad_key->GetServiceType().value(), item->public_ip.c_str());
        }
    }
    xinfo("GetServiceNeighbours %u of service_type:%llu", nodes_vec.size(), kad_key->GetServiceType().value());
    return nodes_vec;
}

}  // namespace elect

}  // namespace tcash
