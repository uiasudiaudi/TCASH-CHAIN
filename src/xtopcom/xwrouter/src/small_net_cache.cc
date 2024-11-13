// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xwrouter/multi_routing/small_net_cache.h"

#include "xpbase/base/kad_key/kadmlia_key.h"
#include "xpbase/base/tcash_log.h"
#include "xpbase/base/tcash_timer.h"
#include "xwrouter/multi_routing/multi_routing.h"
#include "xwrouter/multi_routing/service_node_cache.h"

#include <array>
#include <cassert>

namespace tcash {

namespace wrouter {
static const uint64_t kClearPeriod = 30ll * 1000ll * 1000ll;                   // 30s check timer
static const uint64_t kClearMarkExpiredPeriod = 3ll * 60ll * 1000ll * 1000ll;  // 3 min check timer
static const uint32_t kKeepMarkExpiredMaxTime = 10 * 60 * 1000;                // 10min
static const size_t kMaxSizePerServiceType = 512;

SmallNetNodes * SmallNetNodes::Instance() {
    static SmallNetNodes ins;
    return &ins;
}

bool SmallNetNodes::Init() {
    service_nodes_ = ServiceNodes::Instance();

    tcash_INFO("SmallNetNodes Init");
    return true;
}

SmallNetNodes::SmallNetNodes() {
}

SmallNetNodes::~SmallNetNodes() {
    // clear_timer_->Join();
    // clear_timer_ = nullptr;
    tcash_INFO("SmallNetNodes destroy");
}

void SmallNetNodes::GetAllServiceType(std::set<base::ServiceType> & svec) {
    std::unique_lock<std::mutex> lock(net_nodes_cache_map_mutex_);
    for (auto & item : net_nodes_cache_map_) {
        svec.insert(item.first);
    }
    tcash_DEBUG("getallservicetype size: %d", svec.size());
    return;
}

bool SmallNetNodes::FindRandomNode(WrouterTableNode & Fnode, base::ServiceType service_type) {
    std::unique_lock<std::mutex> lock(net_nodes_cache_map_mutex_);

    for (auto const & _p : net_nodes_cache_map_) {
        if (_p.first == service_type && _p.second.size()) {
            auto size = _p.second.size();
            uint32_t index = RandomUint32() % size;
            Fnode = _p.second[index];
            tcash_DEBUG("findnode of index:%d account:%s", index, Fnode.node_id.c_str());
            return true;
        }
    }
    return false;
}

bool SmallNetNodes::FindAllNode(std::vector<WrouterTableNode> & node_vec, base::ServiceType service_type) {
    std::unique_lock<std::mutex> lock(net_nodes_cache_map_mutex_);

    for (auto const & _p : net_nodes_cache_map_) {
        if (_p.first == service_type && _p.second.size()) {
            node_vec.clear();
            for (auto & node : _p.second) {
                node_vec.push_back(node);
            }
            return true;
        }
    }
    return false;
}

void SmallNetNodes::GetAllNode(std::vector<WrouterTableNode> & node_vec) {
    std::unique_lock<std::mutex> lock(net_nodes_cache_map_mutex_);
    for (const auto & item : net_nodes_cache_map_) {
        for (auto & node : item.second) {
            node_vec.push_back(node);
        }
    }
    return;
}

void SmallNetNodes::AddNode(std::vector<wrouter::WrouterTableNode> node) {
    if (node.empty())
        return;
    base::ServiceType comming_service_type = base::GetKadmliaKey(node[0].m_xip2)->GetServiceType();
    std::unique_lock<std::mutex> lock(net_nodes_cache_map_mutex_);

    if (comming_service_type.IsBroadcastService()) {
        if (net_nodes_cache_map_.find(comming_service_type) != net_nodes_cache_map_.end()) {
            xdbg("ElectNetNodes::AddNode update broadcast service election result %s", comming_service_type.info().c_str());
            net_nodes_cache_map_.erase(comming_service_type);
        }
    } else {
        std::vector<base::ServiceType> erase_service_vector;
        bool save_two_round{true};
        for(auto riter = net_nodes_cache_map_.rbegin();riter!=net_nodes_cache_map_.rend();riter++){
            base::ServiceType service_type = riter->first;
            if(comming_service_type.IsNewer(service_type)){
                if(save_two_round){
                    save_two_round = false;
                    continue;
                }
                xdbg("ElectNetNodes::AddNode get new election result %s erase old %s", comming_service_type.info().c_str(), service_type.info().c_str());
                service_nodes_->RemoveExpired(service_type);
                erase_service_vector.push_back(service_type);
            }
        }
        for (auto & erase_service : erase_service_vector) {
            net_nodes_cache_map_.erase(erase_service);
        }
    }

    net_nodes_cache_map_.insert(std::make_pair(comming_service_type, node));
}

}  // end namespace wrouter

}  // end namespace tcash
